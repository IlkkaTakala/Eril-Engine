#include "Physics.h"
#include <Objects/Terrain.h>
#include "Objects/MovementComponent.h"

MovementComponent::MovementComponent()
{
	mass = 1.f;
	in_acceleration = 600.f;
	max_speed = 5.f;
	isPhysics = false;
	isGravity = true;
	inAir = false;
	allowMovement = true;
	direction_count = 0;
	force_count = 0;
	drag = 1.f;
	brake = 2000.f;
	air_control = 0.05f;
	Physics::AddMovable(this);
}

void MovementComponent::OnDestroyed()
{
	Physics::RemoveMovable(this);
}

void MovementComponent::Tick(float time)
{
	if (!allowMovement) return;
	DesiredState.location = Object->GetLocation();
	OldState = DesiredState;
	if (Object == nullptr) return;
	switch (isPhysics)
	{
	case true:
	{
		const Vector gravity(0.f, 0.f, -9.8f);
		Vector delta;
		Vector delta_a;
		Vector velocity;
		Vector brake_a;

		if (isGravity) {
			DesiredState.acceleration += gravity;
			velocity += DesiredState.acceleration;
			//velocity += gravity;
		}
		
		for (int i = 0; i < force_count; i++) {
			delta_a += forces[i].Direction;
		}

		velocity = OldState.velocity + delta_a * time + brake_a;

		//float drag = velocity.LengthSquared() * (0.5f / max_speed);

		//velocity -= velocity.Normalize() * drag;

		//if (velocity.LengthSquared() > max_speed * max_speed) velocity = velocity.Normalize() * max_speed;

		//if (velocity.LengthSquared() < 0.01f) velocity = 0.f;

		DesiredState.location = Object->GetLocation() + velocity;
		if (!inAir && Terra != nullptr) {
			velocity.Z = 0.f;
			DesiredState.location.Z = Terra->GetHeight(DesiredState.location.X, DesiredState.location.Y);
		}
		DesiredState.velocity = velocity;

	}
	break;
	
	case false:
	{
		Vector delta_a;
		Vector velocity;
		Vector brake_a;
		if (direction_count > 0) {
			for (int i = 0; i < direction_count; i++) {
				delta_a += directions[i];
			}
			delta_a = delta_a.Normalize();
			delta_a *= in_acceleration * (inAir ? air_control : 1.f);
		}
		else {
			brake_a = -DesiredState.velocity.Normalize() * brake * time;
			brake_a = brake_a.LengthSquared() > DesiredState.velocity.LengthSquared() ? -DesiredState.velocity : brake_a;
		}
		for (int i = 0; i < force_count; i++) {
			delta_a += forces[i].Direction;
		}

		velocity = OldState.velocity + delta_a * time + brake_a;

		//float drag = velocity.LengthSquared() * (0.5f / max_speed);

		//velocity -= velocity.Normalize() * drag;

		if (velocity.LengthSquared() > max_speed * max_speed) velocity = velocity.Normalize() * max_speed;
		
		if (velocity.LengthSquared() < 0.01f) velocity = 0.f;

		DesiredState.location = Object->GetLocation() + velocity * time;
		if (!inAir && Terra != nullptr) {
			velocity.Z = 0.f;
			DesiredState.location.Z = Terra->GetHeight(DesiredState.location.X, DesiredState.location.Y);
		}

		DesiredState.velocity = velocity;
	}
	break;
	}
	direction_count = 0;
	force_count = 0;

}

void MovementComponent::SetTarget(Actor* t)
{
	Object = t;
	//Physics::RemoveStatic(t);
}

void MovementComponent::SetGround(Terrain* t)
{
	Terra = t;
}

void MovementComponent::ApplyMovement()
{
	if (!allowMovement) return;
	Object->SetLocation(DesiredState.location);
}
