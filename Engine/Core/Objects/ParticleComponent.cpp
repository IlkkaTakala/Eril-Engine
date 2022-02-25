#include "ParticleComponent.h"
#include <RenderCore/ParticleSystem.h>

ParticleComponent::ParticleComponent()
{
	system = SpawnObject<ParticleSystem>();
	system->Create(this);
}

ParticleComponent::~ParticleComponent()
{
}

void ParticleComponent::SetSystem(ParticleSystem* s)
{
	system = s;
}

ParticleSystem* ParticleComponent::GetSystem() const
{
	return system;
}

void ParticleComponent::Tick(float delta)
{
	system->Update(delta);
}
