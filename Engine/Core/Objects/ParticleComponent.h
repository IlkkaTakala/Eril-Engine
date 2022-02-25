#pragma once
#include <Objects/SceneComponent.h>

class ParticleSystem;

class ParticleComponent : public SceneComponent, public Tickable
{
public:
	ParticleComponent();
	virtual ~ParticleComponent();
	void SetSystem(ParticleSystem* s);
	ParticleSystem* GetSystem() const;
	virtual void Tick(float delta) override;

private:
	bool autoDestroy;
	bool loop;

	Ref<ParticleSystem> system;
};

