#pragma once
#include "sims.h"
#include "math/vector3.h"
#include "math/bbox.h"
#include "core/color.h"
#include "graphics_api/sims_sdk_d3d9.h"
using namespace sims;

struct Particle
{
	Vector3f position;
	Color color;
	static const uint32 FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
};

struct Attribute
{
	Vector3f position;
	Vector3f velocity;
	Vector3f acceleration;
	// how long the particle lives for before dying
	float lifeTime;
	// current age of particle
	float age;
	// current color of particle
	Color color;
	// how the color fades with respect to time
	Color colorFade;
	bool isAlive;
};

class ParticleSystem
{
public:
	ParticleSystem();
	virtual ~ParticleSystem();

	virtual bool Init(IDirect3DDevice9* device, const char* texFilename);
	virtual void Reset();
	// some time we don't want free the memory of a dead particle,
	// but rather re-spawn it instead
	virtual void ResetParticle(Attribute* attr) = 0;
	
	virtual void AddParticle();

	virtual void Update(const Timestep& timestep) = 0;
	virtual void PreRender();
	virtual void Render();
	virtual void PostRender();

	bool isEmpty() const;
	bool isDead() const;
protected:
	virtual void RemoveDeadParticles();

	IDirect3DDevice9* device_;
	IDirect3DTexture9* texture_;
	IDirect3DVertexBuffer9* vb_;

	// all particles emit from here
	Vector3f origin_;
	// bounding box of moving range of particle
	BBox bbox_;
	// new particles emit per second
	float emitRate_;
	// size of particle
	float size_;
	// max allowed particles system can have
	uint32 maxParticles_;
	std::list<Attribute> particles_;

	// following used for rendering particle system efficiently
	// size of vb
	uint32 vbSize_;
	// offset in vb to lock
	uint32 vbOffset_;
	// number of vertices to lock starting at vbOffset
	uint32 vbBatchSize_;
};

