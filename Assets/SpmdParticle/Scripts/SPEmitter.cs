using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace SpmdParticle
{
    [AddComponentMenu("SpmdParticle/Emitter")]
    public class SPEmitter : MonoBehaviour
    {

        static List<SPEmitter> instances = new List<SPEmitter>();

        public enum Shape
        {
            Sphere,
            Box,
        }

        public SPWorld[] m_targets;
        public Shape m_shape = Shape.Sphere;
        public float m_emit_count = 100.0f;
        public Vector3 m_velosity_base = Vector3.zero;
        public float m_velosity_random_diffuse = 0.5f;
        public float m_lifetime = 30.0f;
        public float m_lifetime_random_diffuse = 1.0f;
        public int m_userdata;
        public SPHitHandler m_spawn_handler = null;
        SPSpawnParams m_params;
        float m_emit_count_prev;
        float m_local_time;
        int m_total_emit;


        delegate void TargetEnumerator(SPWorld world);
        void EachTargets(TargetEnumerator e)
        {
            if (m_targets.Length != 0)
                foreach (var w in m_targets) e(w);
            else
                foreach (var w in SPWorld.s_instances) e(w);
        }


        void OnEnable()
        {
            instances.Add(this);
        }

        void OnDisable()
        {
            instances.Remove(this);
        }

        public void SPUpdate()
        {
            if (m_emit_count_prev != m_emit_count)
            {
                m_emit_count_prev = m_emit_count;
                m_total_emit = Mathf.FloorToInt(m_local_time * m_emit_count);
            }
            m_local_time += Time.deltaTime;
            int emit_total = Mathf.FloorToInt(m_local_time * m_emit_count);
            int emit_this_frame = emit_total - m_total_emit;
            if (emit_this_frame == 0) return;
            m_total_emit = emit_total;

            m_params.velocity = m_velosity_base;
            m_params.velocity_random_diffuse = m_velosity_random_diffuse;
            m_params.lifetime = m_lifetime;
            m_params.lifetime_random_diffuse = m_lifetime_random_diffuse;
            m_params.userdata = m_userdata;
            m_params.handler = m_spawn_handler;
            Matrix4x4 mat = transform.localToWorldMatrix;
            switch (m_shape)
            {
                case Shape.Sphere:
                    EachTargets((w) =>
                    {
                        SPAPI.spScatterParticlesSphereTransform(w.GetContext(), ref mat, emit_this_frame, ref m_params);
                    });
                    break;

                case Shape.Box:
                    EachTargets((w) =>
                    {
                        SPAPI.spScatterParticlesBoxTransform(w.GetContext(), ref mat, emit_this_frame, ref m_params);
                    });
                    break;
            }
        }

        public static void SPUpdateAll()
        {
            foreach (var o in instances)
            {
                if (o != null && o.enabled) o.SPUpdate();
            }
        }

        void OnDrawGizmos()
        {
            if (!enabled) return;
            Gizmos.color = Color.cyan;
            Gizmos.matrix = transform.localToWorldMatrix;
            switch (m_shape)
            {
                case Shape.Sphere:
                    Gizmos.DrawWireSphere(Vector3.zero, 0.5f);
                    break;

                case Shape.Box:
                    Gizmos.DrawWireCube(Vector3.zero, Vector3.one);
                    break;
            }
            Gizmos.matrix = Matrix4x4.identity;
        }
    }

}
