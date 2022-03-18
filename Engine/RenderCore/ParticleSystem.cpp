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

	Dirty = false;
	FaceCamera = true;
	Sprite = nullptr;
	Type = 0;

	Active = false;

	Spawner = nullptr;
	Material = nullptr;
	Parent = nullptr;
	Constructor = nullptr;
	Updator = nullptr;

	Autoplay = false;
}

void ParticleSystem::ApplyMaterial()
{
	if (MaterialBuffer == 0) {
		glGenBuffers(1, &MaterialBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, MaterialBuffer);
		glBufferData(GL_ARRAY_BUFFER, MaxParticleCount * sizeof(MaterialParams), nullptr, GL_DYNAMIC_DRAW);
	}
	if (Dirty) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, MaterialBuffer);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, MatData.size() * sizeof(MaterialParams), MatData.data());
	}

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, MaterialBuffer);
}

ParticleSystem::~ParticleSystem()
{
	delete Spawner;
	delete Sprite;
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
	Parent = attach;

	if (!Sprite) {
		Sprite = MI->GetStatic(attach, "sprite");
		Sprite->SetAABB(AABB({ -10 }, { 10 }));
	}
	else Sprite->SetParent(attach);
	if (!Sprite) return;
	if (!Material) {
		Material = IRender::LoadMaterialByName("Assets/Materials/sprite");
	}
	Sprite->SetMaterial(0, Material);
	Sprite->SetInstances(MaxParticleCount, Transforms.data());
	Sprite->SetInstanceCount(ParticleCount);
	Sprite->SetBinds(std::bind(&ParticleSystem::ApplyMaterial, this));

	
}

void ParticleSystem::Update(float delta)
{
	if (!Active || !Spawner || !Constructor) return;
	
	Spawner->Check(delta);

	int t_idx = 0;
	int last_active = 0;

	if (Updator) Updator(this, delta);

	MatData.resize(ParticleCount);
	for (uint i = 0; i <= ParticleCount; i++) {
		if (!Particles[i].enabled) continue;

		Particle& p = Particles[i];

		Transforms[t_idx].Scale = p.scale;
		Transforms[t_idx].Location = p.location;
		if (FaceCamera) {
			Vector cam = IRender::GetActiveCamera()->GetRotation();
			Transforms[t_idx].Rotation.Z = cam.Z;
			Transforms[t_idx].Rotation.Y = cam.Y;
			Transforms[t_idx].Rotation.X = p.rotation.X;
		}
		else Transforms[t_idx].Rotation = p.rotation;

		MatData[t_idx] = { glm::vec4(p.colour.X, p.colour.Y, p.colour.Z, p.alpha), {p.lifetime, p.lifetime / p.max_lifetime, 0.f, 0.f} };

		t_idx++;
		last_active = i;
	}
	Sprite->SetInstances(t_idx, Transforms.data());
	MatData.resize(t_idx);
	Dirty = true;

	ParticleCount = last_active;
}

bool ParticleSystemConstruction::RateSpawner::Check(float delta)
{
	if (system->ParticleCount >= system->MaxParticleCount) return false;
	SpawnLast += delta;
	if (SpawnLast < SpawnInterval) return false;
	SpawnLast = 0.f;
	uint32 idx = 0;
	if (system->FreeIdx.empty())
		idx = ++system->ParticleCount;
	else {
		idx = system->FreeIdx.front();
		system->FreeIdx.pop_front();
	}
	if (idx >= system->Particles.size()) return false;
	system->Constructor(system->Particles[idx]);
	system->Particles[idx].location += system->Parent->GetWorldLocation();
	system->Particles[idx].rotation += system->Parent->GetWorldRotation();
	system->Particles[idx].enabled = true;
	system->Particles[idx].lifetime = 0.f;
	system->ParticleCount = system->ParticleCount > idx ? system->ParticleCount : idx;
	return true;
}

void ParticleSystemConstruction::Updator::UpdateVelocities(float delta) const
{
	for (uint i = 0; i <= system->ParticleCount; i++) {
		Particle& p = system->Particles[i];

		if (!p.enabled) continue;

		p.location += p.velocity * delta;
		p.rotation += p.rotationRate * delta;
		if (p.rotation.X > 360.f)
			p.rotation.X -= 360.f;
		else if (p.rotation.X < 0.f)
			p.rotation.X += 360.f;
		if (p.rotation.Y > 360.f)
			p.rotation.Y -= 360.f;
		else if (p.rotation.Y < 0.f)
			p.rotation.Y += 360.f;
		if (p.rotation.Z > 360.f)
			p.rotation.Z -= 360.f;
		else if (p.rotation.Z < 0.f)
			p.rotation.Z += 360.f;
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
			system->FreeIdx.push_back(i);
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
	for (uint i = 0; i <= system->ParticleCount; i++) {
		Particle& p = system->Particles[i];
		if (!p.enabled) continue;

		p.scale = p.initialScale * curve.EvaluateCurve(p.lifetime / p.max_lifetime);

	}
}

void ParticleSystemConstruction::Updator::SpriteRotationRate(float delta, const CurveData& curve) const
{
	for (uint i = 0; i <= system->ParticleCount; i++) {
		Particle& p = system->Particles[i];
		if (!p.enabled) continue;

		p.rotation.X += delta * curve.EvaluateCurve(p.lifetime / p.max_lifetime);
		if (p.rotation.X > 360.f)
			p.rotation.X -= 360.f;
	}
}

void ParticleSystemConstruction::Updator::Color(float delta, const VectorCurveData& curve) const
{
	for (uint i = 0; i <= system->ParticleCount; i++) {
		Particle& p = system->Particles[i];
		if (!p.enabled) continue;

		p.colour = p.initialColour * curve.EvaluateCurve(p.lifetime / p.max_lifetime);

	}
}
