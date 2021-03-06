#include "stdafx.h"

#include "SpBase.h"
#include "SpWorld.h"
#include "SpmdParticle.h"

namespace {

	std::vector<spWorld*> g_worlds;
}

extern "C" {


	spAPI void spUpdateDataTexture(int context, void *tex, int width, int height)
	{
		
		if (context == 0) return;
		g_worlds[context]->updateDataTexture(tex, width, height);
	}



	spAPI int spCreateContext()
	{
		
		spWorld *p = new spWorld();

		if (g_worlds.empty()) {
			g_worlds.push_back(nullptr);
		}
		for (int i = 1; i < (int)g_worlds.size(); ++i) {
			if (g_worlds[i] == nullptr) {
				g_worlds[i] = p;
				return i;
			}
		}
		g_worlds.push_back(p);
		return (int)g_worlds.size() - 1;
	}

	spAPI void spDestroyContext(int context)
	{
		
		delete g_worlds[context];
		g_worlds[context] = nullptr;
	}


	spAPI void spUpdate(int context, float dt)
	{		
		g_worlds[context]->update(dt);
	}

	spAPI void spBeginUpdate(int context, float dt)
	{
		
		g_worlds[context]->beginUpdate(dt);
	}

	spAPI void spEndUpdate(int context)
	{
		
		g_worlds[context]->endUpdate();
	}

	spAPI void spCallHandlers(int context)
	{
		
		g_worlds[context]->callHandlers();
	}


	spAPI void spClearParticles(int context)
	{
		
		g_worlds[context]->clearParticles();
	}

	spAPI void spClearCollidersAndForces(int context)
	{
		
		g_worlds[context]->clearCollidersAndForces();
	}

	spAPI void spGetKernelParams(int context, spKernelParams *params)
	{
		
		*params = g_worlds[context]->getKernelParams();
	}

	spAPI void spSetKernelParams(int context, const spKernelParams *params)
	{
		
		g_worlds[context]->setKernelParams(*params);
	}


	spAPI int spGetNumParticles(int context)
	{
		
		if (context == 0) return 0;
		return g_worlds[context]->getNumParticles();
	}

	spAPI void spForceSetNumParticles(int context, int num)
	{
		
		g_worlds[context]->forceSetNumParticles(num);
	}
	spAPI spParticleIM*	spGetIntermediateData(int context, int nth)
	{
		
		return nth < 0 ?
			&g_worlds[context]->getIntermediateData() :
			&g_worlds[context]->getIntermediateData(nth);
	}

	spAPI spParticle* spGetParticles(int context)
	{
		
		return g_worlds[context]->getParticles();
	}


	inline void spApplySpawnParams(spParticleCont &particles, const spSpawnParams *params)
	{
		
		if (params == nullptr) return;

		vec3 vel = params->velocity_base;
		float vel_diffuse = params->velocity_random_diffuse;
		float lifetime = params->lifetime;
		float lifetime_diffuse = params->lifetime_random_diffuse;
		int userdata = params->userdata;
		spHitHandler handler = params->handler;

		for (auto &p : particles) {
			(vec3&)p.velocity = vec3(
				vel.x + spGenRand()*vel_diffuse,
				vel.y + spGenRand()*vel_diffuse,
				vel.z + spGenRand()*vel_diffuse);
			p.lifetime = lifetime + spGenRand()*lifetime_diffuse;
			p.userdata = userdata;
		}
		if (handler) {
			for (auto &p : particles) {
				handler(&p);
			}
		}
	}

	spAPI void spScatterParticlesSphere(int context, vec3 *center, float radius, int32_t num, const spSpawnParams *params)
	{
		
		if (num <= 0) { return; }

		spParticleCont particles(num);
		for (auto &p : particles) {
			float l = spGenRand()*radius;
			vec3 dir = glm::normalize(vec3(spGenRand(), spGenRand(), spGenRand()));
			vec3 pos = *center + dir * l;
			(vec3&)p.position = pos;
		}
		spApplySpawnParams(particles, params);
		g_worlds[context]->addParticles(particles.data(), particles.size());
	}

	spAPI void spScatterParticlesBox(int context, vec3 *center, vec3 *size, int32_t num, const spSpawnParams *params)
	{
		
		if (num <= 0) { return; }

		spParticleCont particles(num);
		for (auto &p : particles) {
			vec3 pos = *center + vec3(spGenRand()*size->x, spGenRand()*size->y, spGenRand()*size->z);
			(vec3&)p.position = pos;
		}
		spApplySpawnParams(particles, params);
		g_worlds[context]->addParticles(particles.data(), particles.size());
	}


	spAPI void spScatterParticlesSphereTransform(int context, mat4 *transform, int32_t num, const spSpawnParams *params)
	{
		
		if (num <= 0) { return; }

		spParticleCont particles(num);
		simdmat4 mat(*transform);
		for (auto &p : particles) {
			vec3 dir = glm::normalize(vec3(spGenRand(), spGenRand(), spGenRand()));
			float l = spGenRand()*0.5f;
			simdvec4 pos = simdvec4(dir*l, 1.0f);
			pos = mat * pos;
			(vec3&)p.position = (vec3&)pos;
		}
		spApplySpawnParams(particles, params);
		g_worlds[context]->addParticles(particles.data(), particles.size());
	}

	spAPI void spScatterParticlesBoxTransform(int context, mat4 *transform, int32_t num, const spSpawnParams *params)
	{
		
		if (num <= 0) { return; }

		spParticleCont particles(num);
		simdmat4 mat(*transform);
		for (auto &p : particles) {
			simdvec4 pos(spGenRand()*0.5f, spGenRand()*0.5f, spGenRand()*0.5f, 1.0f);
			pos = mat * pos;
			(vec3&)p.position = (vec3&)pos;
		}
		spApplySpawnParams(particles, params);
		g_worlds[context]->addParticles(particles.data(), particles.size());
	}



	inline void spBuildBoxCollider(int context, spBoxCollider &o, const mat4 &transform, const vec3 &center, const vec3 &_size)
	{
		

		float psize = g_worlds[context]->getKernelParams().particle_size;
		vec3 size = _size * 0.5f;

		simdmat4 st = simdmat4(transform);
		simdvec4 vertices[] = {
			simdvec4(size.x + center.x, size.y + center.y, size.z + center.z, 0.0f),
			simdvec4(-size.x + center.x, size.y + center.y, size.z + center.z, 0.0f),
			simdvec4(-size.x + center.x, -size.y + center.y, size.z + center.z, 0.0f),
			simdvec4(size.x + center.x, -size.y + center.y, size.z + center.z, 0.0f),
			simdvec4(size.x + center.x, size.y + center.y, -size.z + center.z, 0.0f),
			simdvec4(-size.x + center.x, size.y + center.y, -size.z + center.z, 0.0f),
			simdvec4(-size.x + center.x, -size.y + center.y, -size.z + center.z, 0.0f),
			simdvec4(size.x + center.x, -size.y + center.y, -size.z + center.z, 0.0f),
		};
		for (int i = 0; i < mpCountof(vertices); ++i) {
			vertices[i] = st * vertices[i];
		}

		simdvec4 normals[6] = {
			glm::normalize(glm::cross(vertices[3] - vertices[0], vertices[4] - vertices[0])),
			glm::normalize(glm::cross(vertices[5] - vertices[1], vertices[2] - vertices[1])),
			glm::normalize(glm::cross(vertices[7] - vertices[3], vertices[2] - vertices[3])),
			glm::normalize(glm::cross(vertices[1] - vertices[0], vertices[4] - vertices[0])),
			glm::normalize(glm::cross(vertices[1] - vertices[0], vertices[3] - vertices[0])),
			glm::normalize(glm::cross(vertices[7] - vertices[4], vertices[5] - vertices[4])),
		};
		float distances[6] = {
			-(glm::dot(vertices[0], normals[0]) + psize),
			-(glm::dot(vertices[1], normals[1]) + psize),
			-(glm::dot(vertices[0], normals[2]) + psize),
			-(glm::dot(vertices[3], normals[3]) + psize),
			-(glm::dot(vertices[0], normals[4]) + psize),
			-(glm::dot(vertices[4], normals[5]) + psize),
		};

		(vec3&)o.shape.center = (vec3&)transform[3][0];
		for (int i = 0; i < 6; ++i) {
			(vec3&)o.shape.planes[i].normal = (vec3&)normals[i];
			o.shape.planes[i].distance = distances[i];
		}
		for (int i = 0; i < mpCountof(vertices); ++i) {
			vec3 p = (vec3&)vertices[i] + (vec3&)o.shape.center;
			if (i == 0) {
				(vec3&)o.bounds.bl = p;
				(vec3&)o.bounds.ur = p;
			}
			(vec3&)o.bounds.bl = glm::min((vec3&)o.bounds.bl, p - psize);
			(vec3&)o.bounds.ur = glm::max((vec3&)o.bounds.ur, p + psize);
		}
	}

	inline void spBuildSphereCollider(int context, spSphereCollider &o, vec3 center, float radius)
	{

		float psize = g_worlds[context]->getKernelParams().particle_size;
		float er = radius + psize;
		(vec3&)o.shape.center = center;
		o.shape.radius = er;
		(vec3&)o.bounds.bl = center - er;
		(vec3&)o.bounds.ur = center + er;
	}


	spAPI void spAddBoxCollider(int context, spColliderProperties *props, mat4 *transform, vec3 *size, vec3 *center)
	{
		

		spBoxCollider col;
		col.props = *props;
		spBuildBoxCollider(context, col, *transform, *size, *center);
		g_worlds[context]->addBoxColliders(&col, 1);
	}

	spAPI void spRemoveCollider(int context, spColliderProperties *props)
	{
		
		g_worlds[context]->removeCollider(*props);
	}



	spAPI void spAddForce(int context, spForceProperties *props, mat4 *_trans)
	{
		
		mat4 &trans = *_trans;
		spForce force;
		force.props = *props;

		switch ((spForceShape)force.props.shape_type) {
		case spForceShape::Sphere:
		{
			spSphereCollider col;
			vec3 pos = (vec3&)trans[3];
			float radius = (trans[0][0] + trans[1][1] + trans[2][2]) * 0.3333333333f * 0.5f;
			spBuildSphereCollider(context, col, pos, radius);
			force.bounds = col.bounds;
			force.sphere.center = col.shape.center;
			force.sphere.radius = col.shape.radius;
		}
		break;



		case spForceShape::Box:
		{
			spBoxCollider col;
			spBuildBoxCollider(context, col, trans, vec3(), vec3(1.0f, 1.0f, 1.0f));
			force.bounds = col.bounds;
			force.box.center = col.shape.center;
			for (int i = 0; i < 6; ++i) {
				force.box.planes[i] = col.shape.planes[i];
			}
		}
		break;

		}
		g_worlds[context]->addForces(&force, 1);
	}


} // extern "C"
