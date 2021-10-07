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
	brake = 2000.f;
	air_control = 0.2f;
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

		const Vector drag_a = Vector(DesiredState.velocity.X, DesiredState.velocity.Y, 0.f).Normalize() * brake * time;

		const Vector total_a = delta_a - drag_a;

		DesiredState.acceleration = total_a;

		DesiredState.velocity += DesiredState.acceleration * time;
		if (DesiredState.velocity.Length() > max_speed) DesiredState.velocity = DesiredState.velocity.Normalize() * max_speed;
		else if (DesiredState.velocity.Length() < 0.01f) DesiredState.velocity = Vector(0.f);

		if (isGravity && inAir) {
			const Vector gravity_const(0.f, 0.f, -9.8f);
			DesiredState.velocity += gravity_const * time;
		}
		
		DesiredState.location = Object->GetLocation() + (DesiredState.velocity) * time;

		if (Terra != nullptr) {
			float height = Terra->GetHeight(DesiredState.location.X, DesiredState.location.Y);
			
			if (DesiredState.location.Z < height) {
				DesiredState.location.Z = height;
				if (DesiredState.velocity.Z < 0.f) DesiredState.velocity.Z = 0.f;
				if (DesiredState.acceleration.Z < 0.f) DesiredState.acceleration.Z = 0.f;
				inAir = false;
			}
			else {
				if (DesiredState.location.Z > height)
					inAir = true;
			}
		}
	}
	break;
	}
	direction_count = 0;
	force_count = 0;

	printf("X: %.2f, Y: %.2f, Z: %.2f\n", DesiredState.acceleration.X, DesiredState.acceleration.Y, DesiredState.acceleration.Z);
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
