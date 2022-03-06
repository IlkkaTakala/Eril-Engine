#include "CloudParticle.h"
#include "RenderCore/Particle.h"
#include <CurveData.h>

CloudParticle::CloudParticle() : ParticleSystem()
{
	auto spri = MI->LoadData(nullptr, "sprite");
	spri->SetAABB(AABB({-50}, {50}));
	SetModel(spri);
	SetMaterial(RI->LoadMaterialByName("Assets/Materials/cloud_sprite"));
	SetMaxCount(200);

	SetConstructor([](Particle& p) {
		p.initialScale = RandomFloatInRange(100, 180);
		p.velocity = Vector(1.0, 1.0, 0.0).Normalize();//Vector::RandomUnitVectorInCone({ 0, 0, 1 }, 90.f, { 0, 1, 0 }) * 0.3;
		p.rotationRate = Vector(RandomFloatInRange(-2, 2), 0.f, 0.f);
		p.rotation = RandomFloatInRange(0, 360);
		p.max_lifetime = 25.f;
		p.location = { RandomFloatInRange(-45, 45), RandomFloatInRange(-45, 45), RandomFloatInRange(-15, 15) };
		p.alpha = 1.f;
		p.initialColour = Vector(0.9);
		p.colour = Vector(1.0);
		});

	SetUpdator([](ParticleSystem* system, float delta) {
		ParticleSystemConstruction::Updator u(system);
		u.UpdateLifetime(delta);
		u.UpdateVelocities(delta);
		u.Alpha(delta, CurveData({ {0.f, 0.f}, {0.2f, 1.f}, { 1.f, 0.f } }));
		//u.Color(delta, VectorCurveData({ {0.f, {0.9f}} }));
		u.SpriteSize(delta, CurveData({ {0.f, 1.f}, {1.f, 1.5f} }));
		});

	SetSpawner(ParticleSystemConstruction::MakeSpawnRate(this, 4.0f));
}
