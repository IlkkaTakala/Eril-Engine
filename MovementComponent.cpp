#include "Objects/Actor.h"
#include "Objects/MovementComponent.h"

MovementComponent::MovementComponent()
{
	mass = 1.f;
	speed = 10.f;
	in_acceleration = 100.f;
	max_speed = 10.f;
	isPhysics = false;
	isGravity = true;
	direction_count = 0;
	drag = 1.f;
	brake = 1000.f;
}

void MovementComponent::Tick(float time)
{
	if (Object == nullptr) return;
	switch (isPhysics)
	{
	case true:
	{
		const Vector gravity(0.f, 0.f, -9.8f);
		Vector delta;

		if (isGravity) acceleration += gravity * time;
		velocity += acceleration * time;
		delta = velocity * time;
		Object->AddLocation(delta);
	}
	break;

	case false:
	{
		Vector delta_a;
		for (int i = 0; i < direction_count; i++) {
			delta_a += directions[i];
		}
		delta_a = delta_a.Normalize();
		delta_a *= in_acceleration;

		const Vector drag_a = velocity.Normalize() * brake * time;

		const Vector total_a = delta_a - drag_a;

		acceleration = total_a;

		if (isGravity) {
			const Vector gravity_const(0.f, 0.f, -9.8f);
			gravity += gravity_const * time;
		}
		velocity = velocity + acceleration * time;
		if (velocity.Length() > max_speed) velocity = velocity.Normalize() * max_speed;
		printf("%.1f\n", velocity.Length());
		Object->AddLocation((velocity + gravity) * time);
	}
	break;
	}
	direction_count = 0;
}
