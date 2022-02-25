#include "ParticleSystem.h"

struct Particle
{
	Vector location;
	Vector scale;
	Vector rotation;
	Vector velocity;
	float lifetime;
	float max_lifetime;
	bool enabled;
};

ParticleSystem::ParticleSystem()
{
	MaxParticleCount = 100;
	ParticleCount = 0;

	dirty = false;
	FaceCamera = true;
	sprite = nullptr;

	constructor = []() {
		Particle p;
		p.scale = { 0.1, 0.1, 0.1 };
		p.velocity = Vector(rand(), rand(), rand()).Normalize();
		p.max_lifetime = 3.f;
		p.enabled = false;
		p.lifetime = 0.f;
		p.location = 0.f;

		return p;
	};
	SpawnDelay = 0.5f;
	SpawnCurrent = 0.f;
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::Create(SceneComponent* attach)
{
	Transforms.resize(MaxParticleCount);
	Particles.resize(MaxParticleCount);

	sprite = MI->LoadData(attach, "Cube");
	if (!sprite) return;
	sprite->SetMaterial(0, RI->LoadMaterialByName("Assets/Materials/default"));
	sprite->SetInstances(MaxParticleCount, Transforms.data());
	sprite->SetInstanceCount(ParticleCount);

}

void ParticleSystem::Update(float delta)
{
	SpawnCurrent += delta;
	if (SpawnCurrent >= SpawnDelay && ParticleCount < MaxParticleCount) {
		SpawnCurrent = 0.f;
		int idx = 0;
		if (freeIdx.empty())
			idx = ParticleCount;
		else {
			idx = freeIdx.front();
			freeIdx.pop_front();
		}
		Particles[idx] = constructor(); 
		Particles[idx].enabled = true;
		ParticleCount++;
	}

	int t_idx = 0;
	for (int i = 0; i < ParticleCount; i++) {
		if (!Particles[i].enabled) continue;
		Particle& p = Particles[i];
		

		p.location += p.velocity * delta;


		Transforms[t_idx].Scale = p.scale;
		Transforms[t_idx].Location = p.location;
		Transforms[t_idx].Rotation = p.rotation;

		t_idx++;

		p.lifetime += delta;
		if (p.lifetime >= p.max_lifetime)
		{
			p.enabled = false;
			ParticleCount--;
			freeIdx.push_back(i);
		}
	}
	dirty = true;
	if (dirty)
	{
		dirty = false;
		sprite->SetInstances(ParticleCount, Transforms.data());
	}
}
