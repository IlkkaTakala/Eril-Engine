#include "Physics.h"
#include "Objects/MovementComponent.h"

MovementComponent::MovementComponent()
{
	mass = 1.f;
	in_acceleration = 100.f;
	max_speed = 10.f;
	isPhysics = false;
	isGravity = true;
	direction_count = 0;
	drag = 1.f;
	brake = 1000.f;
	Physics::AddMovable(this);
}

void MovementComponent::OnDestroyed()
{
	Physics::RemoveMovable(this);
}

void MovementComponent::Tick(float time)
{
	OldState = DesiredState;
	if (Object == nullptr) return;
	switch (isPhysics)
	{
	case true:
	{
		const Vector gravity(0.f, 0.f, -9.8f);
		Vector delta;

		if (isGravity) DesiredState.acceleration += gravity * time;
		DesiredState.velocity += DesiredState.acceleration * time;
		delta = DesiredState.velocity * time;
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

		const Vector drag_a = DesiredState.velocity.Normalize() * brake * time;

		const Vector total_a = delta_a - drag_a;

		DesiredState.acceleration = total_a;

		if (isGravity) {
			const Vector gravity_const(0.f, 0.f, -9.8f);
			DesiredState.gravity += gravity_const * time;
		}
		else DesiredState.gravity = 0.f;

		DesiredState.velocity = DesiredState.velocity + DesiredState.acceleration * time;
		if (DesiredState.velocity.Length() > max_speed) DesiredState.velocity = DesiredState.velocity.Normalize() * max_speed;
		else if (DesiredState.velocity.Length() < 0.1f) DesiredState.velocity = Vector(0.f);
		
		DesiredState.location = Object->GetLocation() + (DesiredState.velocity + DesiredState.gravity) * time;
	}
	break;
	}
	direction_count = 0;
}

void MovementComponent::SetTarget(Actor* t)
{
	Object = t;
	Physics::RemoveStatic(t);
}

void MovementComponent::ApplyMovement()
{
	Object->SetLocation(DesiredState.location);
}
