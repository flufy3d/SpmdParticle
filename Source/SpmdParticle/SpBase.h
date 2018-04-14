#pragma once

#include "spCore_ispc.h"
#include "SoA.h"

#ifdef _MSC_VER
#   define mpCountof(exp) _countof(exp)
#else
#   define mpCountof(exp) (sizeof(exp)/sizeof(exp[0]))
#endif

typedef int16_t         i16;
typedef uint16_t        u16;
typedef int32_t         i32;
typedef uint32_t        u32;
typedef float           f32;

typedef __m128 simd128;

using ist::vec4soa2;
using ist::vec4soa3;
using ist::vec4soa4;

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::ivec2;
using glm::ivec3;
using glm::ivec4;
using glm::mat3;
using glm::mat4;
typedef glm::simdVec4 simdvec4;
typedef glm::simdMat4 simdmat4;

typedef ispc::Context                   spKernelContext;
typedef ispc::Cell                      spCell;

typedef ispc::Plane                     spPlane;
typedef ispc::Sphere                    spSphere;
typedef ispc::Box                       spBox;

typedef ispc::ColliderProperties        spColliderProperties;
typedef ispc::PlaneCollider             spPlaneCollider;
typedef ispc::SphereCollider            spSphereCollider;
typedef ispc::BoxCollider               spBoxCollider;

typedef ispc::ForceProperties           spForceProperties;
typedef ispc::Force                     spForce;

namespace glm {
	inline float length_sq(const vec2 &v) { return dot(v, v); }
	inline float length_sq(const vec3 &v) { return dot(v, v); }
	inline float length_sq(const vec4 &v) { return dot(v, v); }
} // namespace glm


template<class Scalar>
inline Scalar clamp(Scalar v, Scalar min, Scalar max)
{
	return std::min<Scalar>(std::max<Scalar>(v, min), max);
}

inline int msb(int a)
{
#ifdef _MSC_VER
	unsigned long r;
	_BitScanReverse(&r, (unsigned long)a);
	return (int)r;
#else  // _MSC_VER
	return a == 0 ? 0 : 31 - __builtin_clz(a);
#endif // _MSC_VER
}

// -1.0f-1.0f
float spGenRand();

// 0.0f-1.0f
float spGenRand1();


struct spKernelParams : ispc::KernelParams
{
	spKernelParams()
	{
		(vec3&)world_center = vec3(0.0f, 0.0f, 0.0f);
		(vec3&)world_extent = vec3(10.24f, 10.24f, 10.24f);
		(ivec3&)world_div = ivec3(128, 128, 128);
		(vec3&)coord_scaler = vec3(1.0f, 1.0f, 1.0f);

		enable_interaction = 1;
		enable_colliders = 1;
		enable_forces = 1;
		id_as_float = 0;

		timestep = 1.0f / 60.0f;
		damping = 0.6f;
		advection = 0.5f;
		pressure_stiffness = 500.0f;

		max_particles = 100000;
		particle_size = 0.08f;

	}
};

struct spTempParams
{
	vec3 cell_size;
	vec3 rcp_cell_size;
	vec3 world_bounds_bl;
	vec3 world_bounds_ur;
	ivec3 world_div_bits;
};


struct spParticle
{
	union {
		simd128 position;
		struct {
			vec3 position3f;
			u32 id;
		};
	};
	simd128 velocity;
	union {
		u32 hash;
		f32 density;
	};
	f32 lifetime;
	struct {
		u16 hit;
		u16 hit_prev;
	};
	int userdata;

	spParticle() {}
	spParticle(const spParticle &v) { *this = v; }
	spParticle& operator=(const spParticle &v)
	{
		simd128 *dst = (simd128*)this;
		simd128 *src = (simd128*)&v;
		dst[0] = src[0];
		dst[1] = src[1];
		dst[2] = src[2];
		return *this;
	}
};

// intermediate data
struct spParticleIM
{
	simd128 accel;
};

struct spParticleForce
{
	int num_hits;
	int pad[3];
	simd128 position;
	simd128 force;

	spParticleForce() { clear(); }
	void clear() { memset(this, 0, sizeof(*this)); }
	spParticleForce& operator=(const spParticleForce &v)
	{
		simd128 *dst = (simd128*)this;
		simd128 *src = (simd128*)&v;
		dst[0] = src[0];
		dst[1] = src[1];
		dst[2] = src[2];
		return *this;
	}
};
typedef void(__stdcall *spHitHandler)(spParticle *p);
typedef void(__stdcall *spForceHandler)(spParticleForce *p);

struct spSpawnParams
{
	vec3 velocity_base;
	float velocity_random_diffuse;
	float lifetime;
	float lifetime_random_diffuse;
	int userdata;
	spHitHandler handler;
};

void* spAlignedAlloc(size_t size, size_t align);
void spAlignedFree(void *p);


template<typename T, int Align = 32>
class spAlignedAllocator {
public:
	typedef T value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	template<typename U> struct rebind { typedef spAlignedAllocator<U> other; };
public:
	spAlignedAllocator() {}
	spAlignedAllocator(const spAlignedAllocator&) {}
	template<typename U> spAlignedAllocator(const spAlignedAllocator<U>&) {}
	~spAlignedAllocator() {}
	pointer address(reference r) { return &r; }
	const_pointer address(const_reference r) { return &r; }
	pointer allocate(size_type cnt, const void *p = nullptr) { return (pointer)spAlignedAlloc(cnt * sizeof(T), Align); }
	void deallocate(pointer p, size_type) { spAlignedFree(p); }
	size_type max_size() const { return std::numeric_limits<size_type>::max() / sizeof(T); }
	void construct(pointer p, const T& t) { new(p)T(t); }
	void destroy(pointer p) { p->~T(); }
	bool operator==(spAlignedAllocator const&) { return true; }
	bool operator!=(spAlignedAllocator const& a) { return !operator==(a); }
};
template<class T, typename Alloc> inline bool operator==(const spAlignedAllocator<T>& l, const spAlignedAllocator<T>& r) { return (l.equals(r)); }
template<class T, typename Alloc> inline bool operator!=(const spAlignedAllocator<T>& l, const spAlignedAllocator<T>& r) { return (!(l == r)); }

typedef std::vector<float, spAlignedAllocator<float> >                          spFloatArray;
typedef std::vector<int, spAlignedAllocator<int> >                              spIntArray;
typedef std::vector<spParticle, spAlignedAllocator<spParticle> >                spParticleCont;
typedef std::vector<spParticleIM, spAlignedAllocator<spParticleIM> >            spParticleIMCont;
typedef std::vector<spParticleForce, spAlignedAllocator<spParticleForce> >      spPForceCont;
typedef std::vector<spCell, spAlignedAllocator<spCell> >                            spCellCont;
typedef std::vector<spColliderProperties*, spAlignedAllocator<spPlaneCollider> >    spColliderPropertiesCont;
typedef std::vector<spPlaneCollider, spAlignedAllocator<spPlaneCollider> >      spPlaneColliderCont;
typedef std::vector<spSphereCollider, spAlignedAllocator<spSphereCollider> >    spSphereColliderCont;
typedef std::vector<spBoxCollider, spAlignedAllocator<spBoxCollider> >          spBoxColliderCont;
typedef std::vector<spForce, spAlignedAllocator<spForce> >                      spForceCont;

struct spSoAData
{
	struct Reference
	{
		vec3 pos;
		vec3 vel;
		vec3 acl;
		float speed;
		float density;
		float affection;
		int hit;
	};

	spFloatArray pos_x;
	spFloatArray pos_y;
	spFloatArray pos_z;
	spFloatArray vel_x;
	spFloatArray vel_y;
	spFloatArray vel_z;
	spFloatArray acl_x;
	spFloatArray acl_y;
	spFloatArray acl_z;
	spFloatArray speed;
	spFloatArray density;
	spFloatArray affection;
	spIntArray hit;

	void resize(size_t n);
};


