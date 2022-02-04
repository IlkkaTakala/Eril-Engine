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
	Physics::RemoveBody(body);
}

void ColliderComponent::LoadWithParameters(const String& args)
{
	body = Physics::addBox(1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 10, 0);
	Physics::AddCollider(this);
}

void ColliderComponent::SetType(int t)
{
	type = t;
	switch (type)
	{
	case 0:
		body->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
		body->setMassProps(0, btVector3(0.f , 0.f, 0.f));
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
	btBoxShape* box = new btBoxShape(btVector3(width / 2.f, height / 2.f, depth / 2.f));
	body->setCollisionShape(box);
}

void ColliderComponent::SetMass(float m)
{
	
}

void ColliderComponent::SetTarget(MovementComponent* m)
{
	Object = m;
}

void ColliderComponent::SetLocation(const Vector& NewLocation, bool force)
{
	SceneComponent::SetLocation(NewLocation, force);
	Vector loc = GetWorldLocation();
	body->getWorldTransform().setOrigin(btVector3(loc.X, loc.Z, loc.Y));
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
	
	switch (type)
	{
	case 0:
		if (Object) {
			//Vector velocity = Object->DesiredState.velocity;
			//body->setLinearVelocity(btVector3(velocity.X, velocity.Z, velocity.Y));
		}
		break;
	case 1:
		if (Object) {
			Vector velocity = Object->DesiredState.velocity;
			body->setLinearVelocity(btVector3(velocity.X, velocity.Z, velocity.Y));
		}
		break;
	case 2:
		if (Object) {
			Vector velocity = Object->DesiredState.velocity;
			body->setLinearVelocity(btVector3(velocity.X, velocity.Z, velocity.Y));
		}
		break;
	}
}

void ColliderComponent::ApplyCollision()
{
	switch (type)
	{
	case 0:
		if (Object) {
			auto l = body->getWorldTransform().getOrigin();
			Vector world(l[0], l[2], l[1]);
			Object->DesiredState.location = world - Location;
			Object->DesiredState.velocity = Vector(0.f);
		}
		break;
	case 1:
		if (Object) {
			auto l = body->getWorldTransform().getOrigin();
			Vector world(l[0], l[2], l[1]);
			Object->DesiredState.location = world - Location;
		}
		break;
	case 2:
		if (Object) {
			auto l = body->getWorldTransform().getOrigin();
			Vector world(l[0], l[2], l[1]);
			Object->DesiredState.location = world - Location;
		}
		break;
	}
}

