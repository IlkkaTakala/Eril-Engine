#include "ColliderComponent.h"
#include <Physics/BulletPhysics.h>
#include <Physics.h>
#include "MovementComponent.h"


ColliderComponent::ColliderComponent()
{
	body = nullptr;
	type = 0;
}

void ColliderComponent::OnDestroyed()
{
	Physics::RemoveCollider(this);
}

void ColliderComponent::BeginPlay()
{
	body = Physics::addBox(1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 10, 1);
	Physics::AddCollider(this);
}

void ColliderComponent::SetType(int t)
{
	type = t;
	switch (type)
	{
	case 0:
		body->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
		break;
	case 1:
		body->setCollisionFlags(btCollisionObject::CF_DYNAMIC_OBJECT);
		break;
	case 2:
		body->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
		break;
	}
}

void ColliderComponent::SetSize(AABB s)
{
	float width = s.maxs.X - s.mins.X;
	float height = s.maxs.Z - s.mins.Z;
	float depth = s.maxs.Y - s.mins.Y;
	size = s;
	btBoxShape* box = new btBoxShape(btVector3(width, height, depth));
	body->setCollisionShape(box);
}

void ColliderComponent::SetTarget(MovementComponent* m)
{
	Object = m;
}

void ColliderComponent::SetLocation(const Vector& NewLocation, bool force)
{
	SceneComponent::SetLocation(NewLocation, force);
	body->getWorldTransform().setOrigin(btVector3(NewLocation.X, NewLocation.Z, NewLocation.Y));
}

void ColliderComponent::SetRotation(const Vector& NewRotation, bool force)
{
	SceneComponent::SetRotation(NewRotation, force);
	body->getWorldTransform().setRotation(btQuaternion(NewRotation.Y, NewRotation.Z, NewRotation.X));
}

void ColliderComponent::Tick(float Delta)
{
	Vector location = GetWorldLocation();
	btTransform colliderloc;
	colliderloc.setIdentity();
	colliderloc.setOrigin(btVector3(location.X, location.Z, location.Y));
	body->setWorldTransform(colliderloc);
	
	if (type == 1) {
		if (Object) {
			Vector velocity = Object->DesiredState.velocity;
			body->setLinearVelocity(btVector3(velocity.X, velocity.Z, velocity.Y));
		}
	}
}

void ColliderComponent::ApplyCollision()
{
	if (type == 1) {
		if (Object) {
			auto l = body->getWorldTransform().getOrigin();
			Object->DesiredState.location = Vector(l[0], l[2], l[1]);
		}
	}
}

