#pragma once
#include <Objects/SceneComponent.h>
#include <RenderCore/ParticleSystem.h>

class ParticleComponent : public SceneComponent, public Tickable
{
public:
	ParticleComponent();
	virtual ~ParticleComponent();
	virtual void BeginPlay() override;
	void SetSystem(ParticleSystem* s);
	ParticleSystem* GetSystem() const;
	virtual void Tick(float delta) override;

private:
	bool autoDestroy;
	bool loop;

	Ref<ParticleSystem> system;
};

