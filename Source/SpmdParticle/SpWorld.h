#pragma once
#include "SpConcurrency.h"

class spWorld
{
public:

    spWorld();
    ~spWorld();
    void beginUpdate(float dt);
    void endUpdate();
    void update(float dt);
    void callHandlers();

    void addParticles(spParticle *p, size_t num);
    void addPlaneColliders(spPlaneCollider *col, size_t num);
    void addSphereColliders(spSphereCollider *col, size_t num);
    void addBoxColliders(spBoxCollider *col, size_t num);
    void removeCollider(spColliderProperties &props);
    void addForces(spForce *force, size_t num);



    void clearParticles();
    void clearCollidersAndForces();

    const spKernelParams&   getKernelParams() const;
    void                    setKernelParams(const spKernelParams &v);
    spTempParams&           getTempParams();
    const spCellCont&       getCells();

    void        forceSetNumParticles(int v);
    int         getNumParticles() const;
    spParticle* getParticles();
    int         getNumParticlesGPU() const;
    spParticle* getParticlesGPU();

    spParticleIM& getIntermediateData(int i);
    spParticleIM& getIntermediateData();

    std::mutex& getMutex();

    int updateDataTexture(void *tex, int width, int height);

private:
    typedef ist::combinable<spPForceCont> spPForceConbinable;

    spParticleCont          m_particles;
    spParticleIMCont        m_imd;
    spSoAData               m_soa;
    spCellCont              m_cells;
    u32                     m_id_seed;
    int                     m_num_particles;

    spColliderPropertiesCont m_collider_properties;
    spPlaneColliderCont     m_plane_colliders;
    spSphereColliderCont    m_sphere_colliders;

    spBoxColliderCont       m_box_colliders;
    spForceCont             m_forces;
    bool                    m_has_hithandler;
    bool                    m_has_forcehandler;

    ist::task_group         m_taskgroup;
    std::mutex              m_mutex;
    spKernelParams          m_kparams;
    spTempParams            m_tparams;

    spPForceCont            m_pforce;
    spPForceConbinable      m_pcombinable;

    int                     m_num_particles_gpu;
    int                     m_num_particles_gpu_prev;
    spParticleCont          m_particles_gpu;

    int                     m_current;
};
