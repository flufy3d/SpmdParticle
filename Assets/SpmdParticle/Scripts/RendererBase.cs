using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Threading;

namespace SpmdParticle
{

public abstract class RendererBase : MonoBehaviour
{
    public int m_max_instances = 1024 * 4;
    public Mesh m_mesh;
    public Material m_material;
    public Material[] m_materials;
    public LayerMask m_layer_selector = 1;
    public bool m_cast_shadow = false;
    public bool m_receive_shadow = false;
    public Vector3 m_scale = Vector3.one;
    public Camera m_camera;
    public bool m_flush_on_LateUpdate = true;
    public Vector3 m_bounds_size = Vector3.one;

    protected int m_instances_par_batch;
    protected int m_instance_count;
    protected int m_batch_count;
    protected int m_layer;
    protected Transform m_trans;
    protected Mesh m_expanded_mesh;
    protected List< List<Material> >m_actual_materials;

    public int GetMaxInstanceCount() { return m_max_instances; }
    public int GetInstanceCount() { return m_instance_count; }
    public void SetInstanceCount(int v) { m_instance_count = v; }



    public abstract Material CloneMaterial(Material src, int nth);
    public abstract void UpdateGPUResources();

    public const int max_vertices = 65000; // Mesh's limitation
    public const int data_texture_width = 128;
    public const int max_data_transfer_size = 64768;

    public static int ceildiv(int v, int d)
    {
        return v / d + (v % d == 0 ? 0 : 1);
    }

    public static Mesh CreateExpandedMesh(Mesh mesh, int required_instances, out int instances_par_batch)
    {
        Vector3[] vertices_base = mesh.vertices;
        Vector3[] normals_base = (mesh.normals == null || mesh.normals.Length == 0) ? null : mesh.normals;
        Vector4[] tangents_base = (mesh.tangents == null || mesh.tangents.Length == 0) ? null : mesh.tangents;
        Vector2[] uv_base = (mesh.uv == null || mesh.uv.Length == 0) ? null : mesh.uv;
        Color[] colors_base = (mesh.colors == null || mesh.colors.Length == 0) ? null : mesh.colors;
        int[] indices_base = (mesh.triangles == null || mesh.triangles.Length == 0) ? null : mesh.triangles;
        instances_par_batch = Mathf.Min(max_vertices / mesh.vertexCount, required_instances);

        Vector3[] vertices = new Vector3[vertices_base.Length * instances_par_batch];
        Vector2[] idata = new Vector2[vertices_base.Length * instances_par_batch];
        Vector3[] normals = normals_base == null ? null : new Vector3[normals_base.Length * instances_par_batch];
        Vector4[] tangents = tangents_base == null ? null : new Vector4[tangents_base.Length * instances_par_batch];
        Vector2[] uv = uv_base == null ? null : new Vector2[uv_base.Length * instances_par_batch];
        Color[] colors = colors_base == null ? null : new Color[colors_base.Length * instances_par_batch];
        int[] indices = indices_base == null ? null : new int[indices_base.Length * instances_par_batch];

        for (int ii = 0; ii < instances_par_batch; ++ii)
        {
            for (int vi = 0; vi < vertices_base.Length; ++vi)
            {
                int i = ii * vertices_base.Length + vi;
                vertices[i] = vertices_base[vi];
                idata[i] = new Vector2((float)ii, (float)vi);
            }
            if (normals != null)
            {
                for (int vi = 0; vi < normals_base.Length; ++vi)
                {
                    int i = ii * normals_base.Length + vi;
                    normals[i] = normals_base[vi];
                }
            }
            if (tangents != null)
            {
                for (int vi = 0; vi < tangents_base.Length; ++vi)
                {
                    int i = ii * tangents_base.Length + vi;
                    tangents[i] = tangents_base[vi];
                }
            }
            if (uv != null)
            {
                for (int vi = 0; vi < uv_base.Length; ++vi)
                {
                    int i = ii * uv_base.Length + vi;
                    uv[i] = uv_base[vi];
                }
            }
            if (colors != null)
            {
                for (int vi = 0; vi < colors_base.Length; ++vi)
                {
                    int i = ii * colors_base.Length + vi;
                    colors[i] = colors_base[vi];
                }
            }
            if (indices != null)
            {
                for (int vi = 0; vi < indices_base.Length; ++vi)
                {
                    int i = ii * indices_base.Length + vi;
                    indices[i] = ii * vertices_base.Length + indices_base[vi];
                }
            }

        }
        Mesh ret = new Mesh();
        ret.vertices = vertices;
        ret.normals = normals;
        ret.tangents = tangents;
        ret.uv = uv;
        ret.colors = colors;
        ret.uv2 = idata;
        ret.triangles = indices;
        return ret;
    }
       
    public void ForEachEveryMaterials(System.Action<Material> a)
    {
        m_actual_materials.ForEach(
            (ma) => { 
                ma.ForEach(v => { a(v); });
            });
    }

    protected void ClearMaterials()
    {
        m_actual_materials.ForEach(a => { a.Clear(); });
    }

    protected virtual void IssueDrawCall()
    {
        Matrix4x4 matrix = Matrix4x4.identity;
        m_actual_materials.ForEach(a =>
        {
            for (int i = 0; i < m_batch_count; ++i)
            {
                Graphics.DrawMesh(m_expanded_mesh, matrix, a[i], m_layer, m_camera, 0, null, m_cast_shadow, m_receive_shadow);
            }
        });
    }


    public virtual void Flush()
    {
        if (m_expanded_mesh == null || m_instance_count == 0)
        {
            m_instance_count = 0;
            return;
        }

        Vector3 scale = m_trans.localScale;
        m_expanded_mesh.bounds = new Bounds(m_trans.position,
            new Vector3(m_bounds_size.x * scale.x, m_bounds_size.y * scale.y, m_bounds_size.y * scale.y));
        m_instance_count = Mathf.Min(m_instance_count, m_max_instances);
        m_batch_count = ceildiv(m_instance_count, m_instances_par_batch);

        for (int i = 0; i < m_actual_materials.Count; ++i )
        {
            var a = m_actual_materials[i];
            while (a.Count < m_batch_count)
            {
                Material m = CloneMaterial(m_materials[i], a.Count);
                a.Add(m);
            }
        }
        UpdateGPUResources();
        IssueDrawCall();
        m_instance_count = m_batch_count = 0;
    }



    public virtual void OnEnable()
    {
        m_trans = GetComponent<Transform>();

        if (m_materials==null || m_materials.Length==0)
        {
            m_materials = new Material[1] { m_material };
        }

        m_actual_materials = new List<List<Material>>();
        while (m_actual_materials.Count < m_materials.Length)
        {
            m_actual_materials.Add(new List<Material>());
        }

        if (m_expanded_mesh == null && m_mesh != null)
        {
            m_expanded_mesh = CreateExpandedMesh(m_mesh, m_max_instances, out m_instances_par_batch);
            m_expanded_mesh.UploadMeshData(true);
        }

        int layer_mask = m_layer_selector.value;
        for (int i = 0; i < 32; ++i )
        {
            if ((layer_mask & (1<<i)) != 0)
            {
                m_layer = i;
                m_layer_selector.value = 1 << i;
                break;
            }
        }

    }

    public virtual void OnDisable()
    {
    }

    public virtual void LateUpdate()
    {
        if (m_flush_on_LateUpdate)
        {
            Flush();
        }
    }

    public virtual void OnDrawGizmos()
    {
        Transform t = GetComponent<Transform>();
        Vector3 s = t.localScale;

        Gizmos.color = Color.yellow;
        Gizmos.DrawWireCube(t.position, new Vector3(m_bounds_size.x * s.x, m_bounds_size.y * s.y, m_bounds_size.z * s.z));
    }
}

}