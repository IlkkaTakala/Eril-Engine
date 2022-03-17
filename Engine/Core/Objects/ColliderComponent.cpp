#include "ColliderComponent.h"
#include <Physics/BulletPhysics.h>
#include <Physics.h>
#include "MovementComponent.h"
#include <btBulletDynamicsCommon.h>

void ErilMotion::getWorldTransform(btTransform& centerOfMassWorldTrans) const
{
	if (!m_userPointer->body) return;
	switch (m_userPointer->GetType())
	{
	case 2:
	{
		if (!m_userPointer->moveObject) return;
		Vector loc = m_userPointer->GetMovementTarget()->DesiredState.location;
		Vector rot = m_userPointer->GetMovementTarget()->DesiredState.rotation;
		btTransform local;
		local.setIdentity();
		local.setOrigin(btVector3(loc.X, loc.Z, loc.Y));
		local.setRotation(btQuaternion(radians(rot.Y), radians(rot.Z), radians(rot.X)));
		centerOfMassWorldTrans = local * m_centerOfMassOffset.inverse();
	} break;

	default: 
	{
		Transformation temp = m_userPointer->GetWorldTransformation();
		btTransform local;
		local.setIdentity();
		local.setOrigin(btVector3(temp.Location.X, temp.Location.Z, temp.Location.Y));
		local.setRotation(btQuaternion(radians(temp.Rotation.Y), radians(temp.Rotation.Z), radians(temp.Rotation.X)));
		centerOfMassWorldTrans = local * m_centerOfMassOffset.inverse();
	} break;
	}
}

void ErilMotion::setWorldTransform(const btTransform& centerOfMassWorldTrans)
{
	if (!m_userPointer->body) return;
	btTransform temp = centerOfMassWorldTrans * m_centerOfMassOffset;
	btVector3 loc = temp.getOrigin();
	Vector wLoc = Vector(loc[0], loc[2], loc[1]) - m_userPointer->GetLocation();
	btQuaternion rot2 = temp.getRotation();
	Vector rot;
	rot2.getEulerZYX(rot.Y, rot.Z, rot.X);
	rot.X = degrees(rot.X);
	rot.Y = degrees(rot.Y);
	rot.Z = degrees(rot.Z);
	btVector3 vel = m_userPointer->body->getLinearVelocity();
	btVector3 gravity = m_userPointer->body->getGravity();

	switch (m_userPointer->GetType())
	{
	case 2: {
		if (!m_userPointer->moveObject) return;
		m_userPointer->GetMovementTarget()->DesiredState.location = wLoc;
		m_userPointer->GetMovementTarget()->DesiredState.velocity = { vel[0], vel[2], vel[1] };
		m_userPointer->GetMovementTarget()->DesiredState.gravity = { gravity[0], gravity[2], gravity[1] };

		//m_userPointer->GetMovementTarget()->DesiredState.rotation = rot;
	} break;

	default:
	{
		Transformation local({ loc[0], loc[2], loc[1] }, rot, m_userPointer->GetTarget()->GetScale());
		m_userPointer->GetTarget()->SetTransform(local);
	} break;
	}
}

ColliderComponent::ColliderComponent()
{
	body = nullptr;
	type = 0;
	mass = 0;
}

void ColliderComponent::OnDestroyed()
{
	Physics::RemoveCollider(this);
	Physics::RemoveBody(body);
}

void ColliderComponent::LoadWithParameters(const String& args)
{
	Physics::AddCollider(this);
}

void ColliderComponent::Tick(float delta)
{
	if (type != 2 || !moveObject) return;
	Vector temp = moveObject->DesiredState.velocity;
	body->setLinearVelocity({ temp.X, temp.Z, temp.Y });
	Vector gra = moveObject->DesiredState.gravity;
	body->setGravity(btVector3(gra.X, gra.Z, gra.Y));
}

void ColliderComponent::SetType(int t)
{
	
}

void ColliderComponent::SetMass(float m)
{
	mass = m;
}

void ColliderComponent::SetTarget(SceneComponent* m)
{
	Object = m;
}

void ColliderComponent::SetMovementTarget(MovementComponent* m)
{
	moveObject = m;
}

SceneComponent* ColliderComponent::GetTarget() const
{
	return Object;
}

void ColliderComponent::SetParent(SceneComponent* parent)
{
	Parent = parent;
	Object = parent;
}

void ColliderComponent::Refresh()
{
	if (!body) return;
	Vector loc = GetWorldLocation();
	Vector rot = GetWorldRotation();
	btTransform temp;
	body->getMotionState()->getWorldTransform(temp);
	temp.setOrigin(btVector3(loc.X, loc.Z, loc.Y));
	temp.setRotation(btQuaternion(radians(rot.Y), radians(rot.Z), radians(rot.X)));
	body->setWorldTransform(temp);
	Physics::ForceUpdate(body);
}

void ColliderComponent::OnCollide()
{
	Console::Log("collision detected");
}

