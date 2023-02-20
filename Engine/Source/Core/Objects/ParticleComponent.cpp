#include "ParticleComponent.h"
#include <RenderCore/ParticleSystem.h>

ParticleComponent::ParticleComponent()
{
	system = nullptr;
}

ParticleComponent::~ParticleComponent()
{
}

void ParticleComponent::BeginPlay()
{
	SceneComponent::BeginPlay();

	if (system) system->Reset();
}

void ParticleComponent::SetSystem(ParticleSystem* s)
{
	system = s;
	system->Initialize(this);
	system->Reset();
}

ParticleSystem* ParticleComponent::GetSystem() const
{
	return system;
}

void ParticleComponent::Tick(float delta)
{
	if (system) system->Update(delta);
}
