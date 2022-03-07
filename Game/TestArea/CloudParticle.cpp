#include "CloudParticle.h"
#include "RenderCore/Particle.h"
#include <CurveData.h>

CloudParticle::CloudParticle() : ParticleSystem()
{
	SetMaterial(RI->LoadMaterialByName("Assets/Materials/cloud_sprite"));
	SetMaxCount(200);

	SetConstructor([](Particle& p) {
		p.initialScale = RandomFloatInRange(10, 18);
		p.velocity = Vector::RandomUnitVectorInCone({ 0, 0, 1 }, 20.f, { 0, 1, 0 });
		p.rotationRate = Vector(RandomFloatInRange(-25, 25), 0.f, 0.f);
		p.max_lifetime = 8.f;
		p.enabled = false;
		p.lifetime = 0.f;
		p.location = Vector::RandomUnitVectorInCone({ 0, 0, 1 }, 90.f, { 0, 1, 0 }) * 0.4;
		p.alpha = 1.f;
		p.initialColour = Vector(0.082, 0.133, 0.180);
		});

	SetUpdator([](ParticleSystem* system, float delta) {
		ParticleSystemConstruction::Updator u(system);
		u.UpdateLifetime(delta);
		u.UpdateVelocities(delta);
		u.Alpha(delta, CurveData({ {0.f, 0.f}, {0.2f, 1.f}, { 1.f, 0.f } }));
		u.Color(delta, VectorCurveData({ {0.f, {1.f}} }));
		u.SpriteSize(delta, CurveData({ {0.f, 0.1f}, {1.f, 1.5f} }));
		});

	SetSpawner(ParticleSystemConstruction::MakeSpawnRate(this, 15.f));
}
