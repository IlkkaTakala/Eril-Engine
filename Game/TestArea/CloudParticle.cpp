#include "CloudParticle.h"
#include "RenderCore/Particle.h"
#include <CurveData.h>

CloudParticle::CloudParticle() : ParticleSystem()
{
	SetConstructor([](Particle& p) {
		p.scale = { 0.1, 0.1, 0.1 };
		p.velocity = Vector::RandomUnitVectorInCone({ 0, 0, 1 }, 90.f, { 0, 1, 0 }).Normalize();
		p.max_lifetime = 3.f;
		p.enabled = false;
		p.lifetime = 0.f;
		p.location = 0.f;
		p.alpha = 1.f;
		p.colour = Vector(1.f);
		});

	SetUpdator([](ParticleSystem* system, float delta) {
		ParticleSystemConstruction::Updator u(system);
		u.UpdateLifetime(delta);
		u.UpdateVelocities(delta);
		u.Alpha(delta, std::vector<std::pair<float, float>>{ {0.f, 0.f}, {0.2f, 1.f}, { 1.f, 0.f } });
		});

	SetSpawner(ParticleSystemConstruction::MakeSpawnRate(this, 6.f));
}
