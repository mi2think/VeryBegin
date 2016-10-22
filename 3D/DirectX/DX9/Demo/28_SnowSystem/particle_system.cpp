#include "particle_system.h"

DWORD RSFLOAT_CAST(float f)
{
	return *((DWORD*)&f);
}

ParticleSystem::ParticleSystem()
	: device_(nullptr)
	, texture_(nullptr)
	, vb_(nullptr)
	, emitRate_(0.0f)
	, size_(0.0f)
	, maxParticles_(0)
	, vbSize_(0)
	, vbOffset_(0)
	, vbBatchSize_(0)
{

}

ParticleSystem::~ParticleSystem()
{
	SAFE_RELEASE(texture_);
	SAFE_RELEASE(vb_);
}

bool ParticleSystem::Init(IDirect3DDevice9* device, const char* texFilename)
{
	device_ = device;

	d3d9::CHECK_HR = device_->CreateVertexBuffer(vbSize_ * sizeof(Particle),
		D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
		Particle::FVF,
		D3DPOOL_DEFAULT,
		&vb_,
		0);
	d3d9::CHECK_HR = D3DXCreateTextureFromFile(device_, texFilename, &texture_);
	return true;
}

void ParticleSystem::Reset()
{
	for (auto& p : particles_)
	{
		ResetParticle(&p);
	}
}

void ParticleSystem::AddParticle()
{
	Attribute attr;
	ResetParticle(&attr);
	particles_.push_back(attr);
}

void ParticleSystem::PreRender()
{
	d3d9::CHECK_HR = device_->SetRenderState(D3DRS_LIGHTING, false);
	// texture coordinates of point primitives are set so that full textures are mapped on each point
	d3d9::CHECK_HR = device_->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	// the point size is interpreted as a camera space value and is scaled by the distance function,
	// and the frustum to viewport y-axis scaling to compute the final screen-space point size. 
	d3d9::CHECK_HR = device_->SetRenderState(D3DRS_POINTSCALEENABLE, true);
	d3d9::CHECK_HR = device_->SetRenderState(D3DRS_POINTSIZE, RSFLOAT_CAST(size_));
	d3d9::CHECK_HR = device_->SetRenderState(D3DRS_POINTSIZE_MIN, RSFLOAT_CAST(0.0f));

	// control size of particle relative to distance
	d3d9::CHECK_HR = device_->SetRenderState(D3DRS_POINTSCALE_A, RSFLOAT_CAST(0.0f));
	d3d9::CHECK_HR = device_->SetRenderState(D3DRS_POINTSCALE_B, RSFLOAT_CAST(0.0f));
	d3d9::CHECK_HR = device_->SetRenderState(D3DRS_POINTSCALE_C, RSFLOAT_CAST(1.0f));

	// use alpha from texture
	d3d9::CHECK_HR = device_->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	d3d9::CHECK_HR = device_->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	d3d9::CHECK_HR = device_->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	d3d9::CHECK_HR = device_->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	d3d9::CHECK_HR = device_->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void ParticleSystem::PostRender()
{
	d3d9::CHECK_HR = device_->SetRenderState(D3DRS_LIGHTING, true);
	d3d9::CHECK_HR = device_->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
}

void ParticleSystem::Render()
{
	// the render method works by filling a section of the vertex buffer with data,
	// then we render that section. while that section is rendering we lock a new section
	// and begin to fill that section. once that sections filled we render it.
	// this process continues until all the particles have been drawn.

	// the benefit of this method is that we keep GPU and CPU busy

	if (!particles_.empty())
	{
		PreRender();

		d3d9::CHECK_HR = device_->SetTexture(0, texture_);
		d3d9::CHECK_HR = device_->SetFVF(Particle::FVF);
		d3d9::CHECK_HR = device_->SetStreamSource(0, vb_, 0, sizeof(Particle));

		// render batches one by one

		// at beginning if we're at the end of the vb

		if (vbOffset_ >= vbSize_)
			vbOffset_ = 0;

		Particle* v = nullptr;
		d3d9::CHECK_HR = vb_->Lock(vbOffset_ * sizeof(Particle),
			vbBatchSize_ * sizeof(Particle),
			(void**)&v,
			vbOffset_ ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);
		
		uint32 numParticlesInBatch = 0;
		// until all particles have been rendered
		for (auto& p : particles_)
		{
			if (p.isAlive)
			{
				// copy a batch of living particles to next vertex buffer segment
				v->position = p.position;
				v->color = p.color;
				++v;
				++numParticlesInBatch;

				// if this batch full?
				if (numParticlesInBatch == vbBatchSize_)
				{
					// draw the last batch of particles that was
					// copied to the vertex buffer
					d3d9::CHECK_HR = vb_->Unlock();

					d3d9::CHECK_HR = device_->DrawPrimitive(D3DPT_POINTLIST,
						vbOffset_, // vertex offset, not bytes
						vbBatchSize_);

					// while the batch is drawing, start filling the next batch with particles

					// move the offset to start of next batch
					vbOffset_ += vbBatchSize_;

					// don't offset into memory thats outsize the vb's range.
					// if we're at the end, starting at beginning
					if (vbOffset_ >= vbSize_)
						vbOffset_ = 0;

					d3d9::CHECK_HR = vb_->Lock(vbOffset_ * sizeof(Particle),
						vbBatchSize_ * sizeof(Particle),
						(void**)&v,
						vbOffset_ ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

					numParticlesInBatch = 0; // reset for new batch
				}
			}
		}
		d3d9::CHECK_HR = vb_->Unlock();

		// it's possible that the Last batch being filled never got rendered.
		// because the condition numParticlesInBatch == vbBatchSize_ would not have been satisfied.
		// we draw the last partially filled batch now
		if (numParticlesInBatch > 0)
		{
			d3d9::CHECK_HR = device_->DrawPrimitive(D3DPT_POINTLIST,
				vbOffset_,
				numParticlesInBatch);
		}

		// next block
		vbOffset_ += vbBatchSize_;

		// reset render state
		PostRender();
	}
}

bool ParticleSystem::isEmpty() const
{
	return particles_.empty();
}

bool ParticleSystem::isDead() const
{
	for (auto& p : particles_)
	{
		if (p.isAlive)
			return false;
	}
	return true;
}

void ParticleSystem::RemoveDeadParticles()
{
	auto it = particles_.begin();
	while (it != particles_.end())
	{
		if (it->isAlive == false)
			it = particles_.erase(it);
		else
			++it;
	}
}
