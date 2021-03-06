#pragma once

#define spAPI __declspec(dllexport)

enum class spForceShape
{
	AffectAll,
	Sphere,
	Capsule,
	Box,
};

enum class spForceType
{
	Directional,
	Radial,
	RadialCapsule,
	Vortex,
	Spline,
	VectorField,
};

typedef vec3    spV3;
typedef ivec3   spV3i;
typedef mat4    spM44;


extern "C" {

	spAPI void           spUpdateDataTexture(int context, void *tex, int width, int height);

	spAPI int            spCreateContext();
	spAPI void           spDestroyContext(int context);
	spAPI void           spUpdate(int context, float dt);
	spAPI void           spBeginUpdate(int context, float dt);   // async version
	spAPI void           spEndUpdate(int context);               // 
	spAPI void           spCallHandlers(int context);

	spAPI void           spClearParticles(int context);
	spAPI void           spClearCollidersAndForces(int context);

	spAPI void           spGetKernelParams(int context, spKernelParams *params);
	spAPI void           spSetKernelParams(int context, const spKernelParams *params);

	spAPI int            spGetNumParticles(int context);
	spAPI void           spForceSetNumParticles(int context, int num);
	spAPI spParticleIM*  spGetIntermediateData(int context, int nth = -1);
	spAPI spParticle*    spGetParticles(int context);
	spAPI void           spAddParticles(int context, spParticle *particles, int num_particles);
	spAPI void           spScatterParticlesSphere(int context, spV3 *center, float radius, int num, const spSpawnParams *params);
	spAPI void           spScatterParticlesBox(int context, spV3 *center, spV3 *size, int num, const spSpawnParams *params);
	spAPI void           spScatterParticlesSphereTransform(int context, spM44 *transform, int num, const spSpawnParams *params);
	spAPI void           spScatterParticlesBoxTransform(int context, spM44 *transform, int num, const spSpawnParams *params);


	spAPI void           spAddBoxCollider(int context, spColliderProperties *props, spM44 *transform, spV3 *center, spV3 *size);
	spAPI void           spRemoveCollider(int context, spColliderProperties *props);
	spAPI void           spAddForce(int context, spForceProperties *p, spM44 *trans);




} // extern "C"



