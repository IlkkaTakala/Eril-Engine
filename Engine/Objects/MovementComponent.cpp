#include "Physics.h"
#include <Objects/Terrain.h>
#include "Objects/MovementComponent.h"

MovementComponent::MovementComponent()
{
	mass = 1.f;
	in_acceleration = 600.f;
	max_speed = 10.f;
	isPhysics = false;
	isGravity = true;
	inAir = true;
	direction_count = 0;
	force_count = 0;
	drag = 1.f;
	brake = 5000.f;
	air_control = 0.05f;
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
		delta_a *= in_acceleration * (inAir ? air_control : 1.f);

		for (int i = 0; i < force_count; i++) {
			delta_a += forces[i].Direction;
		}
		if (delta_a.Z > 0.f) inAir = true;

		const Vector drag_a = inAir ? Vector(0.f) : DesiredState.velocity.Normalize() * brake * time;

		const Vector total_a = delta_a - drag_a;

		DesiredState.acceleration = total_a;

		DesiredState.velocity += DesiredState.acceleration * time;
		Vector temp = DesiredState.velocity;
		temp.Z = 0.f;
		if (temp.Length() > max_speed) temp = temp.Normalize() * max_speed;
		else if (temp.Length() < 0.1f) temp = Vector(0.f);
		DesiredState.velocity.X = temp.X;
		DesiredState.velocity.Y = temp.Y;

		if (isGravity && inAir) {
			const Vector gravity_const(0.f, 0.f, -9.8f);
			DesiredState.velocity += gravity_const * time;
		}
		
		DesiredState.location = Object->GetLocation() + (DesiredState.velocity) * time;

		if (Terra != nullptr) {
			float height = Terra->GetHeight(DesiredState.location.X, DesiredState.location.Y);
			
			if (!inAir) {
				DesiredState.location.Z = height;
				if (DesiredState.velocity.Z < 0.f) DesiredState.velocity.Z = 0.f;
				if (DesiredState.acceleration.Z < 0.f) DesiredState.acceleration.Z = 0.f;
			}
			else {
				if (DesiredState.location.Z <= height)
					inAir = false;
			}
		}
	}
	break;
	}
	direction_count = 0;
	force_count = 0;

}

void MovementComponent::SetTarget(Actor* t)
{
	Object = t;
	Physics::RemoveStatic(t);
}

void MovementComponent::SetGround(Terrain* t)
{
	Terra = t;
}

void MovementComponent::ApplyMovement()
{
	Object->SetLocation(DesiredState.location);
}
