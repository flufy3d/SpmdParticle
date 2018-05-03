using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using SpmdParticle;

public class FrameRateCounter : MonoBehaviour
{
    public float m_update_interval = 0.5f;
    public Text textFPS;
    public Text textParticleNum;
    public int mode = 0;
    private float m_last_time;
    private float m_accum = 0.0f; // FPS accumulated over the interval
    private int m_frames = 0; // Frames drawn over the interval
    private float m_time_left; // Left time for current interval
    private float m_result;
    private ParticleSystem m_SystemParticle;

    void Start()
    {
        m_time_left = m_update_interval;
        m_last_time = Time.realtimeSinceStartup;
        if(mode == 1)
        {
            GameObject hand = GameObject.Find("/Particle System");
            m_SystemParticle = hand.GetComponent<ParticleSystem>(); 
        }

    }

    void Update()
    {
        float now = Time.realtimeSinceStartup;
        float delta = now - m_last_time;
        m_last_time = now;
        m_time_left -= delta;
        m_accum += 1.0f / delta;
        ++m_frames;

        // Interval ended - update result
        if (m_time_left <= 0.0)
        {
            m_result = m_accum / m_frames;
            textFPS.text = "FPS: " + m_result.ToString("f2");
            if(mode == 0)
            {
                textParticleNum.text = "Particles: " + SPWorld.s_instances[0].m_particle_num; 
            }
            else if(mode == 1)
            {
                textParticleNum.text = "Particles: " + m_SystemParticle.particleCount; 
            }
            m_time_left = m_update_interval;
            m_accum = 0.0f;
            m_frames = 0;
        }
    }
}
