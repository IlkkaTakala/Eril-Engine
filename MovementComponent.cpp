#include "Objects/Actor.h"
#include "Objects/MovementComponent.h"

MovementComponent::MovementComponent()
{
	mass = 1.f;
	speed = 10.f;
	max_speed = 10.f;
	isPhysics = false;
	isGravity = true;
	direction_count = 0;
	drag = 1.f;
	brake = 2.f;
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
		Vector delta_v;
		for (int i = 0; i < direction_count; i++) {
			delta_v += directions[i];
		}
		delta_v.Normalize();
		delta_v *= speed;

		const Vector f_drag = -velocity.Normalize() * brake * time;

		Vector accel_v = delta_v + velocity + f_drag;

		const Vector gravity(0.f, 0.f, -9.8f);

		if (isGravity) {
			acceleration += gravity * time;
			accel_v += acceleration;
		}
		velocity = accel_v;
		//delta += velocity * time;
		Object->AddLocation(velocity * time);
	}
	break;
	}
	direction_count = 0;
}
