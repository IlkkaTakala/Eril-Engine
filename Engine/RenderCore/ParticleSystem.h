#pragma once
#include <Core.h>
#include <deque>

struct Particle;
class RenderMesh;

class ParticleSystem : public BaseObject
{
public:
	ParticleSystem();
	virtual ~ParticleSystem();
	virtual void BeginPlay() override {}
	void Create(SceneComponent* attach);
	void Update(float delta);

private:

	bool FaceCamera;
	uint32 ParticleCount;
	uint32 MaxParticleCount;
	float SpawnDelay;
	float SpawnCurrent;

	bool dirty;
	std::deque<int> freeIdx;
	std::vector<Particle> Particles;
	std::vector<Transform> Transforms;

	typedef Particle(*DefaultParticle)();
	DefaultParticle constructor;

	RenderMesh* sprite;
};

