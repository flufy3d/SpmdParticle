using UnityEngine;
using System.Collections;
using System.Collections.Generic;

namespace SpmdParticle
{
    public class SPCollider : MonoBehaviour
    {
        public static List<SPCollider> s_instances = new List<SPCollider>();
        public static List<SPCollider> s_instances_prev = new List<SPCollider>();

        public SPWorld[] m_targets;

        public bool m_receive_hit = false;
        public bool m_receive_force = false;
        public float m_stiffness = 1500.0f;

        public SPHitHandler m_hit_handler;
        public SPForceHandler m_force_handler;
        public SPColliderProperties m_cprops;

        protected Transform m_trans;
        protected Rigidbody m_rigid3d;
        protected Rigidbody2D m_rigid2d;

        protected delegate void TargetEnumerator(SPWorld world);
        protected void EachTargets(TargetEnumerator e)
        {
            if (m_targets.Length != 0)
                foreach (var w in m_targets) e(w);
            else
                foreach (var w in SPWorld.s_instances) e(w);
        }


        public static SPCollider GetHitOwner(int id)
        {
            return s_instances_prev[id];
        }

        void Awake()
        {
            if (m_hit_handler == null) m_hit_handler = PropagateHit;
            if (m_force_handler == null) m_force_handler = PropagateForce;
        }

        void OnEnable()
        {
            m_trans = GetComponent<Transform>();
            m_rigid3d = GetComponent<Rigidbody>();
            m_rigid2d = GetComponent<Rigidbody2D>();
            if (s_instances.Count == 0) s_instances.Add(null);
            s_instances.Add(this);
        }

        void OnDisable()
        {
            s_instances.Remove(this);
            EachTargets((w) =>
            {
                SPAPI.spRemoveCollider(w.GetContext(), ref m_cprops);
            });
        }


        public virtual void SPUpdate()
        {
            m_cprops.stiffness = m_stiffness;
            m_cprops.hit_handler = m_receive_hit ? m_hit_handler : null;
            m_cprops.force_handler = m_receive_force ? m_force_handler : null;
        }

        public static void SPUpdateAll()
        {
            int i = 0;
            foreach (var o in s_instances)
            {
                if (o != null)
                {
                    o.m_cprops.owner_id = i;
                    if (o.enabled) o.SPUpdate();
                }
                ++i;
            }
            s_instances_prev = s_instances;
        }


        public unsafe void PropagateHit(ref SPParticle particle)
        {
            Vector3 f = SPAPI.spGetIntermediateData(SPWorld.GetCurrentContext())->accel * SPWorld.GetCurrent().m_particle_mass;
            if (m_rigid3d != null)
            {
                m_rigid3d.AddForceAtPosition(f, particle.position);
            }
            if (m_rigid2d != null)
            {
                m_rigid2d.AddForceAtPosition(f, particle.position);
            }
        }

        public void PropagateForce(ref SPParticleForce force)
        {
            Vector3 pos = force.position_average;
            Vector3 f = force.force * SPWorld.GetCurrent().m_particle_mass;

            if (m_rigid3d != null)
            {
                m_rigid3d.AddForceAtPosition(f, pos);
            }
            if (m_rigid2d != null)
            {
                m_rigid2d.AddForceAtPosition(f, pos);
            }
        }
    }

}
