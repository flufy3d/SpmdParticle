using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Runtime.Serialization.Formatters.Binary;
using System.IO;

namespace SpmdParticle
{
    [AddComponentMenu("SpmdParticle/World")]
    public class SPWorld : MonoBehaviour
    {
        public static List<SPWorld> s_instances = new List<SPWorld>();
        public static SPWorld s_current;
        static int s_update_count = 0;

        void EachWorld(Action<SPWorld> f)
        {
            s_instances.ForEach(f);
        }
        public static SPWorld GetCurrent() { return s_current; }
        public static int GetCurrentContext() { return s_current.GetContext(); }


        public const int DataTextureWidth = 3072;
        public const int DataTextureHeight = 256;


        public bool m_enable_interaction = true;
        public bool m_enable_colliders = true;
        public bool m_enable_forces = true;
        public bool m_id_as_float = true;
        public float m_particle_mass = 0.1f;
        public float m_timescale = 0.6f;
        public float m_damping = 0.6f;
        public float m_advection = 0.1f;
        public float m_pressure_stiffness = 500.0f;
        public float m_particle_size = 0.08f;
        public int m_max_particle_num = 100000;
        public Vector3 m_coord_scale = Vector3.one;
        public int m_world_div_x = 256;
        public int m_world_div_y = 1;
        public int m_world_div_z = 256;
        public Vector3 m_active_region_center = Vector3.zero;
        public Vector3 m_active_region_extent = Vector3.zero;
        public int m_particle_num = 0;
        public int m_context = 0;

        List<Action> m_actions = new List<Action>();
        List<Action> m_onetime_actions = new List<Action>();
        RenderTexture m_instance_texture;
        bool m_texture_needs_update;



        public int GetContext() { return m_context; }
        public void AddUpdateRoutine(Action a) { m_actions.Add(a); }
        public void RemoveUpdateRoutine(Action a) { m_actions.Remove(a); }
        public void AddOneTimeAction(Action a) { m_onetime_actions.Add(a); }

        public RenderTexture GetInstanceTexture()
        {
            UpdateInstanceTexture();
            return m_instance_texture;
        }

        public void UpdateInstanceTexture()
        {
            if (m_instance_texture == null)
            {
                m_instance_texture = new RenderTexture(SPWorld.DataTextureWidth, SPWorld.DataTextureHeight, 0, RenderTextureFormat.ARGBFloat, RenderTextureReadWrite.Default);
                m_instance_texture.filterMode = FilterMode.Point;
                m_instance_texture.Create();
            }
            if (m_texture_needs_update)
            {
                m_texture_needs_update = false;
                SPAPI.spUpdateDataTexture(GetContext(), m_instance_texture.GetNativeTexturePtr(), m_instance_texture.width, m_instance_texture.height);
            }
        }


        void Awake()
        {
            m_context = SPAPI.spCreateContext();
        }

        void OnDestroy()
        {
            SPAPI.spDestroyContext(GetContext());
            if (m_instance_texture != null)
            {
                m_instance_texture.Release();
                m_instance_texture = null;
            }
        }

        void OnEnable()
        {
            s_instances.Add(this);
        }

        void OnDisable()
        {
            s_instances.Remove(this);
        }



        void Update()
        {
            if (s_update_count++ == 0)
            {
                if (Time.deltaTime != 0.0f)
                {
                    ActualUpdate();
                }
            }
        }


        void LateUpdate()
        {
            --s_update_count;
        }



        static void ActualUpdate()
        {
            if (s_instances.Count == 0) { return; }

            foreach (SPWorld w in s_instances)
            {
                SPAPI.spEndUpdate(w.GetContext());
            }
            foreach (SPWorld w in s_instances)
            {
                s_current = w;
                SPAPI.spCallHandlers(w.GetContext());
                SPAPI.spClearCollidersAndForces(w.GetContext());
                w.CallUpdateRoutines();
                w.UpdateKernelParams();
                s_current = null;
            }
            UpdateSPObjects();
            foreach (SPWorld w in s_instances)
            {
                SPAPI.spBeginUpdate(w.GetContext(), Time.deltaTime);
            }
        }

        void CallUpdateRoutines()
        {
            m_actions.ForEach((a) => { a.Invoke(); });
            m_onetime_actions.ForEach((a) => { a.Invoke(); });
            m_onetime_actions.Clear();
        }


        void OnDrawGizmos()
        {
            if (!enabled) return;
            Gizmos.color = Color.cyan;
            Gizmos.DrawWireCube(transform.position, transform.localScale * 2.0f);
            Gizmos.DrawWireCube(transform.position + m_active_region_center, m_active_region_extent * 2.0f);
        }


        void UpdateKernelParams()
        {
            m_texture_needs_update = true;

            m_particle_num = SPAPI.spGetNumParticles(GetContext());

            SPKernelParams p = default(SPKernelParams);
            SPAPI.spGetKernelParams(GetContext(), ref p);
            p.world_center = transform.position;
            p.world_size = transform.localScale;
            p.world_div_x = m_world_div_x;
            p.world_div_y = m_world_div_y;
            p.world_div_z = m_world_div_z;
            p.active_region_center = transform.position + m_active_region_center;
            p.active_region_extent = m_active_region_extent;
            p.enable_interaction = m_enable_interaction ? 1 : 0;
            p.enable_colliders = m_enable_colliders ? 1 : 0;
            p.enable_forces = m_enable_forces ? 1 : 0;
            p.id_as_float = m_id_as_float ? 1 : 0;
            p.timestep = Time.deltaTime * m_timescale;
            p.damping = m_damping;
            p.advection = m_advection;
            p.pressure_stiffness = m_pressure_stiffness;
            p.scaler = m_coord_scale;
            p.particle_size = m_particle_size;
            p.max_particles = m_max_particle_num;
            SPAPI.spSetKernelParams(GetContext(), ref p);
        }

        static void UpdateSPObjects()
        {
            SPCollider.SPUpdateAll();
            SPForce.SPUpdateAll();
            SPEmitter.SPUpdateAll();
        }

    }

}
