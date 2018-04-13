using UnityEngine;
using System.Collections;
using System.Collections.Generic;

namespace SpmdParticle
{
    [AddComponentMenu("SpmdParticle/Box Collider")]
    public class SPBoxCollider : SPCollider
    {
        public Vector3 m_center;
        public Vector3 m_size = Vector3.one;

        public override void SPUpdate()
        {
            base.SPUpdate();

            Matrix4x4 mat = m_trans.localToWorldMatrix;
            EachTargets((w) =>
            {
                SPAPI.spAddBoxCollider(w.GetContext(), ref m_cprops, ref mat, ref m_center, ref m_size);
            });
        }

        void OnDrawGizmos()
        {
            if (!enabled) return;
            Transform t = GetComponent<Transform>(); 
            Gizmos.color = Color.cyan;
            Gizmos.matrix = t.localToWorldMatrix;
            Gizmos.DrawWireCube(m_center, m_size);
            Gizmos.matrix = Matrix4x4.identity;
        }
    }
}
