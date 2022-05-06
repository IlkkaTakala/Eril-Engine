#include "CloudParticle.h"
#include "RenderCore/Particle.h"
#include <CurveData.h>

CloudParticle::CloudParticle() : ParticleSystem()
{
	//SetMaterial(RI->LoadMaterialByName("Assets/Materials/cloud_sprite"));
	SetMaxCount(200);

	SetConstructor([](Particle& p) {
		p.initialScale = RandomFloatInRange(10, 18);
		p.scale = Vector(1.f);
		p.velocity = Vector::RandomUnitVectorInCone({ 0, 0, 1 }, 20.f, { 0, 1, 0 });
		p.rotationRate = Vector(RandomFloatInRange(-25, 25), 0.f, 0.f);
		p.max_lifetime = 15.f;
		p.enabled = false;
		p.lifetime = 0.f;
		p.location = Vector(0.f, 0.f, 1.f); // ParticleSystemConstruction::Constructor::BoxLocation(p, {10, 10, 1});
		p.alpha = 1.f;
		p.initialColour = Vector(1.f);//Vector(0.082, 0.133, 0.180);
		p.colour = Vector(1.f);//Vector(0.082, 0.133, 0.180);
		p.rotation.W = 0.f;
		});

	SetUpdator([](ParticleSystem* system, float delta) {
		ParticleSystemConstruction::Updator u(system);
		u.UpdateLifetime(delta);
		u.UpdateVelocities(delta);
		u.Alpha(delta, CurveData({ {0.f, 0.f}, {0.2f, 1.f}, { 1.f, 0.f } }));
		//u.ColorBySpeed(delta, VectorCurveData({ {0.f, {1.f, 0.5f, 0.5f}} }));
		u.SpriteSize(delta, CurveData({ {0.f, 0.1f}, {1.f, 1.5f} }));
		//u.AddVelocity(delta, { 1.0f, 0.f, 0.f });
		});

	SetSpawner(ParticleSystemConstruction::MakeSpawnRate(this, 6.0f));
}
