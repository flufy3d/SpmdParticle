using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

namespace SpmdParticle
{
    [StructLayout(LayoutKind.Explicit)]
    public struct SPParticle
    {
        [FieldOffset(0)]
        public Vector3 position;
        [FieldOffset(12)]
        public uint id;
        [FieldOffset(16)]
        public Vector3 velocity;
        [FieldOffset(28)]
        public float speed;
        [FieldOffset(32)]
        public float density;
        [FieldOffset(36)]
        public float lifetime;
        [FieldOffset(40)]
        public UInt16 hit;
        [FieldOffset(42)]
        public UInt16 hit_prev;
        [FieldOffset(44)]
        public int userdata;
    }

    [StructLayout(LayoutKind.Explicit)]
    public struct SPParticleIM
    {
        [FieldOffset(0)]
        public Vector3 accel;
    };

    [StructLayout(LayoutKind.Explicit)]
    public struct SPParticleForce
    {
        [FieldOffset(0)]
        public int num_hits;
        [FieldOffset(16)]
        public Vector3 position_average;
        [FieldOffset(32)]
        public Vector3 force;
    }

    public struct SPKernelParams
    {
        public Vector3 world_center;
        public Vector3 world_size;
        public int world_div_x;
        public int world_div_y;
        public int world_div_z;
        public Vector3 active_region_center;
        public Vector3 active_region_extent;
        public Vector3 scaler;

        public int enable_interaction;
        public int enable_colliders;
        public int enable_forces;
        public int id_as_float;

        public float timestep;
        public float damping;
        public float advection;
        public float pressure_stiffness;

        public int max_particles;
        public float particle_size;
    }

    public delegate void SPHitHandler(ref SPParticle particle);
    public delegate void SPForceHandler(ref SPParticleForce force);



    public struct SPColliderProperties
    {
        public int owner_id;
        public float stiffness;
        public SPHitHandler hit_handler;
        public SPForceHandler force_handler;

        public void SetDefaultValues()
        {
            owner_id = 0;
            stiffness = 1500.0f;
            hit_handler = null;
            force_handler = null;
        }
    }

    public enum SPForceShape
    {
        All,
        Sphere,
        Capsule,
        Box
    }

    public enum SPForceDirection
    {
        Directional,
        Radial,
        RadialCapsule,
        VectorField,
    }

    public struct SPForceProperties
    {
        public SPForceShape shape_type;
        public SPForceDirection dir_type;
        public float strength_near;
        public float strength_far;
        public float range_inner;
        public float range_outer;
        public float rcp_range;
        public float attenuation_exp;

        public float random_seed;
        public float random_diffuse;

        public Vector3 direction;
        public Vector3 center;
        public Vector3 rcp_cellsize;

        public void SetDefaultValues()
        {
            attenuation_exp = 0.25f;
        }
    }

    public unsafe struct SPMeshData
    {
        public int* indices;
        public Vector3* vertices;
        public Vector3* normals;
        public Vector2* uv;
    }

    public struct SPSpawnParams
    {
        public Vector3 velocity;
        public float velocity_random_diffuse;
        public float lifetime;
        public float lifetime_random_diffuse;
        public int userdata;
        public SPHitHandler handler;
    }

    public class SPAPI
    {
        [DllImport("SpmdParticle")]
        public static extern int spUpdateDataTexture(int context, IntPtr tex, int width, int height);

        [DllImport("SpmdParticle")]
        public static extern int spCreateContext();
        [DllImport("SpmdParticle")]
        public static extern void spDestroyContext(int context);
        [DllImport("SpmdParticle")]
        public static extern void spBeginUpdate(int context, float dt);
        [DllImport("SpmdParticle")]
        public static extern void spEndUpdate(int context);
        [DllImport("SpmdParticle")]
        public static extern void spCallHandlers(int context);

        [DllImport("SpmdParticle")]
        public static extern void spClearParticles(int context);
        [DllImport("SpmdParticle")]
        public static extern void spClearCollidersAndForces(int context);
        [DllImport("SpmdParticle")]

        public static extern void spGetKernelParams(int context, ref SPKernelParams p);
        [DllImport("SpmdParticle")]
        public static extern void spSetKernelParams(int context, ref SPKernelParams p);

        [DllImport("SpmdParticle")]
        public static extern int spGetNumParticles(int context);
        [DllImport("SpmdParticle")]
        unsafe public static extern SPParticleIM* spGetIntermediateData(int context, int i = -1);
        [DllImport("SpmdParticle")]
        unsafe public static extern SPParticle* spGetParticles(int context);
        [DllImport("SpmdParticle")]
        public static extern void spScatterParticlesSphere(int context, ref Vector3 center, float radius, int num, ref SPSpawnParams sp);
        [DllImport("SpmdParticle")]
        public static extern void spScatterParticlesSphereTransform(int context, ref Matrix4x4 trans, int num, ref SPSpawnParams sp);
  
        [DllImport("SpmdParticle")]
        public static extern void spScatterParticlesBox(int context, ref Vector3 center, ref Vector3 size, int num, ref SPSpawnParams sp);
        [DllImport("SpmdParticle")]
        public static extern void spScatterParticlesBoxTransform(int context, ref Matrix4x4 trans, int num, ref SPSpawnParams sp);

      
        [DllImport("SpmdParticle")]
        public static extern void spAddBoxCollider(int context, ref SPColliderProperties props, ref Matrix4x4 transform, ref Vector3 center, ref Vector3 size);
        [DllImport("SpmdParticle")]
        public static extern void spRemoveCollider(int context, ref SPColliderProperties props);

        [DllImport("SpmdParticle")]
        public static extern void spAddForce(int context, ref SPForceProperties props, ref Matrix4x4 mat);

    }


}