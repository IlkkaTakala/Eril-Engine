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

	struct Constructor {
		static void BoxLocation(Particle&, Vector size);

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
		void ColorBySpeed(float delta, const VectorCurveData & curve) const;
		void AddVelocity(float delta, const Vector& velocity) const;

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
	void SetMaterial(Material* mat) { Material = mat; }
	void SetModel(RenderMesh* object) { Sprite = object; }
	void SetMaxCount(int count) { MaxParticleCount = count; }

private:
	friend class ParticleSystemConstruction;

	void ApplyMaterial();

	bool Active;
	bool FaceCamera;
	bool Type;
	uint32 ParticleCount;
	uint32 MaxParticleCount;
	bool Autoplay;

	bool Dirty;
	std::deque<int> FreeIdx;
	std::vector<Particle> Particles;
	std::vector<Transform> Transforms;

	typedef void(*DefaultParticle)(Particle&);
	typedef void(*DefaultUpdate)(ParticleSystem*, float delta);
	ParticleSystemConstruction::Spawner* Spawner;
	DefaultParticle Constructor;
	DefaultUpdate Updator;

	RenderMesh* Sprite;
	Material* Material;
	RefWeak<SceneComponent> Parent;

	uint MaterialBuffer;

public:
	void SetSpawner(ParticleSystemConstruction::Spawner* func) { if (Spawner) delete Spawner; Spawner = func; }
	void SetConstructor(DefaultParticle func) { Constructor = func; }
	void SetUpdator(DefaultUpdate func) { Updator = func; }
};