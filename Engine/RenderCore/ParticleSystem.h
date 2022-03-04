#pragma once
#include <Core.h>
#include <deque>

struct Particle;
class RenderMesh;
class ParticleSystemConstruction;
class ParticleSystem;
class CurveData;
class VectorCurveData;

class ParticleSystemConstruction
{
public: 
	struct Spawner {
		Spawner(ParticleSystem* s) : system(s) {}
		ParticleSystem* system;
		virtual bool Check(float) = 0;
	};

	struct Updator {
		Updator(ParticleSystem* s) : system(s) {}
		ParticleSystem* system;

		void UpdateVelocities(float delta) const;
		void UpdateLifetime(float delta) const;
		void Color(float delta, const VectorCurveData& curve) const;
		void Alpha(float delta, const CurveData& curve) const;
		void SpriteSize(float delta, const CurveData & curve) const;
		void SpriteRotationRate(float delta, const CurveData & curve) const;
	};

private:
	ParticleSystemConstruction() {}

	struct RateSpawner : public Spawner {
		RateSpawner(ParticleSystem* s, float rate) : Spawner(s), SpawnInterval(1.f / rate), SpawnLast(0.f) {}
		float SpawnLast;
		float SpawnInterval;
		virtual bool Check(float) override;
	};

public:
	static Spawner* MakeSpawnRate(ParticleSystem* system, float rate) { return new RateSpawner(system, rate); }
};

class ParticleSystem : public BaseObject
{
public:
	virtual ~ParticleSystem();
	virtual void BeginPlay() override { if (Autoplay) Reset(); }
	void Update(float delta);
	void Reset();
	void Initialize(SceneComponent* attach);

	template <typename T>
	static T* MakeSystem() {
		auto next = new T();
		if (auto t = dynamic_cast<ParticleSystem*>(next); !t) delete t;
		return next;
	}
protected:
	ParticleSystem();

private:
	friend class ParticleSystemConstruction;

	void SetMaterial();

	bool Active;
	bool FaceCamera;
	bool Type;
	uint32 ParticleCount;
	uint32 MaxParticleCount;
	bool Autoplay;

	bool dirty;
	std::deque<int> freeIdx;
	std::vector<Particle> Particles;
	std::vector<Transform> Transforms;

	typedef void(*DefaultParticle)(Particle&);
	typedef void(*DefaultUpdate)(ParticleSystem*, float delta);
	ParticleSystemConstruction::Spawner* spawner;
	DefaultParticle constructor;
	DefaultUpdate updator;

	RenderMesh* sprite;

	uint MaterialBuffer;

public:
	void SetSpawner(ParticleSystemConstruction::Spawner* func) { if (spawner) delete spawner; spawner = func; }
	void SetConstructor(DefaultParticle func) { constructor = func; }
	void SetUpdator(DefaultUpdate func) { updator = func; }
};