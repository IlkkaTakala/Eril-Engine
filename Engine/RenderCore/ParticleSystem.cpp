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

	Active = false;

	spawner = nullptr;
	constructor = nullptr;
	updator = nullptr;

	Autoplay = false;
}

ParticleSystem::~ParticleSystem()
{
	delete spawner;
	delete sprite;
}

void ParticleSystem::Reset()
{
	ParticleCount = 0;
	Active = true;
}

void ParticleSystem::Initialize(SceneComponent* attach)
{
	Transforms.resize(MaxParticleCount);
	Particles.resize(MaxParticleCount);

	sprite = MI->LoadData(attach, "Cube");
	if (!sprite) return;
	sprite->SetMaterial(0, RI->LoadMaterialByName("Assets/Materials/default"));
	sprite->SetInstances(MaxParticleCount, Transforms.data());
	sprite->SetInstanceCount(ParticleCount);

	constructor = [](Particle& p) {
		p.scale = { 0.1, 0.1, 0.1 };
		p.velocity = Vector::RandomUnitVectorInCone({ 0, 0, 1 }, 45.f, {0, 1, 0}).Normalize();
		p.max_lifetime = 3.f;
		p.enabled = false;
		p.lifetime = 0.f;
		p.location = 0.f;
	};

	updator = [](ParticleSystem* system, float delta) {
		ParticleSystemConstruction::Updator u(system, delta);
		u.UpdateLifetime();
		u.UpdateVelocities();
	};

	spawner = ParticleSystemConstruction::MakeSpawnRate(this, 0.4f);
}

void ParticleSystem::Update(float delta)
{
	if (!Active || !spawner || !constructor) return;
	if (spawner->Check(delta)) {
		int idx = 0;
		if (freeIdx.empty())
			idx = ++ParticleCount;
		else {
			idx = freeIdx.front();
			freeIdx.pop_front();
		}
		constructor(Particles[idx]);
		Particles[idx].enabled = true;
		ParticleCount = ParticleCount > idx ? ParticleCount : idx;
	}

	int t_idx = 0;
	int last_active = 0;

	if (updator) updator(this, delta);

	for (uint i = 0; i <= ParticleCount; i++) {
		if (!Particles[i].enabled) continue;

		Particle& p = Particles[i];

		Transforms[t_idx].Scale = p.scale;
		Transforms[t_idx].Location = p.location;
		Transforms[t_idx].Rotation = p.rotation;

		t_idx++;
		last_active = i;
	}
	sprite->SetInstances(t_idx, Transforms.data());
	ParticleCount = last_active;
}

bool ParticleSystemConstruction::RateSpawner::Check(float delta)
{
	SpawnLast += delta;
	if (SpawnLast < SpawnInterval) return false;
	SpawnLast = 0.f;
	return true;
}

void ParticleSystemConstruction::Updator::UpdateVelocities() const
{
	for (uint i = 0; i <= system->ParticleCount; i++) {
		Particle& p = system->Particles[i];

		if (!p.enabled) continue;

		p.location += p.velocity * delta;

	}
}

inline void ParticleSystemConstruction::Updator::UpdateLifetime() const
{
	uint last_active = 0;
	for (uint i = 0; i <= system->ParticleCount; i++) {
		Particle& p = system->Particles[i];

		if (!p.enabled) continue;

		p.lifetime += delta;
		if (p.lifetime >= p.max_lifetime)
		{
			p.enabled = false;
			system->freeIdx.push_back(i);
			continue;
		}

		last_active = i;
	}
	system->ParticleCount = last_active;
}
