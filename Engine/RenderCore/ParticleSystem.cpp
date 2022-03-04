#include "ParticleSystem.h"
#include "Objects/SceneComponent.h"
#include "CurveData.h"
#include "Particle.h"

#include <glad/gl.h>
#include <glm/glm.hpp>

struct MaterialParams
{
	glm::vec4 color;
	glm::vec4 ages;
};

ParticleSystem::ParticleSystem()
{
	MaxParticleCount = 100;
	ParticleCount = 0;
	MaterialBuffer = 0;

	dirty = false;
	FaceCamera = true;
	sprite = nullptr;
	Type = 0;

	Active = false;

	spawner = nullptr;
	constructor = nullptr;
	updator = nullptr;

	Autoplay = false;
}

void ParticleSystem::SetMaterial()
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, MaterialBuffer);
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

	sprite = MI->LoadData(attach, "sprite");
	if (!sprite) return;
	sprite->SetMaterial(0, RI->LoadMaterialByName("Assets/Materials/sprite"));
	sprite->SetInstances(MaxParticleCount, Transforms.data());
	sprite->SetInstanceCount(ParticleCount);
	sprite->SetBinds(std::bind(&ParticleSystem::SetMaterial, this));

	glGenBuffers(1, &MaterialBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, MaterialBuffer);
	glBufferData(GL_ARRAY_BUFFER, MaxParticleCount * sizeof(MaterialParams), nullptr, GL_DYNAMIC_DRAW);
}

void ParticleSystem::Update(float delta)
{
	if (!Active || !spawner || !constructor) return;
	if (spawner->Check(delta)) {
		
	}

	int t_idx = 0;
	int last_active = 0;

	if (updator) updator(this, delta);

	std::vector<MaterialParams> params;
	params.reserve(ParticleCount);
	for (uint i = 0; i <= ParticleCount; i++) {
		if (!Particles[i].enabled) continue;

		Particle& p = Particles[i];

		Transforms[t_idx].Scale = p.scale;
		Transforms[t_idx].Location = p.location;
		if (FaceCamera) {
			Vector cam = RI->GetActiveCamera()->GetRotation();
			Transforms[t_idx].Rotation = cam;
			Transforms[t_idx].Rotation.X = p.rotation.X;
			Transforms[t_idx].Rotation.Y = cam.Y + 90.f;
			//Transforms[t_idx].Rotation.Y = cam.X;
		}
		else Transforms[t_idx].Rotation = p.rotation;

		params.push_back({ glm::vec4(p.colour.X, p.colour.Y, p.colour.Z, p.alpha), {p.lifetime, p.lifetime / p.max_lifetime, 0.f, 0.f} });

		t_idx++;
		last_active = i;
	}
	sprite->SetInstances(t_idx, Transforms.data());

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, MaterialBuffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, t_idx * sizeof(MaterialParams), params.data());


	ParticleCount = last_active;
}

bool ParticleSystemConstruction::RateSpawner::Check(float delta)
{
	SpawnLast += delta;
	if (SpawnLast < SpawnInterval) return false;
	SpawnLast = 0.f;
	int idx = 0;
	if (system->freeIdx.empty())
		idx = ++system->ParticleCount;
	else {
		idx = system->freeIdx.front();
		system->freeIdx.pop_front();
	}
	system->constructor(system->Particles[idx]);
	system->Particles[idx].enabled = true;
	system->ParticleCount = system->ParticleCount > idx ? system->ParticleCount : idx;
	return true;
}

void ParticleSystemConstruction::Updator::UpdateVelocities(float delta) const
{
	for (uint i = 0; i <= system->ParticleCount; i++) {
		Particle& p = system->Particles[i];

		if (!p.enabled) continue;

		p.location += p.velocity * delta;

	}
}

void ParticleSystemConstruction::Updator::UpdateLifetime(float delta) const
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

void ParticleSystemConstruction::Updator::Alpha(float delta, const CurveData& curve) const
{
	for (uint i = 0; i <= system->ParticleCount; i++) {
		Particle& p = system->Particles[i];
		if (!p.enabled) continue;

		p.alpha = curve.EvaluateCurve(p.lifetime / p.max_lifetime);
	
	}
}

void ParticleSystemConstruction::Updator::SpriteSize(float delta, const CurveData& curve) const
{
}

void ParticleSystemConstruction::Updator::SpriteRotationRate(float delta, const CurveData& curve) const
{
}

void ParticleSystemConstruction::Updator::Color(float delta, const VectorCurveData& curve) const
{

}
