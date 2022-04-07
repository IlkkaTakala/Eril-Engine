#include "ColliderComponent.h"
#include <Physics/BulletPhysics.h>
#include <Physics.h>
#include "MovementComponent.h"
#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>



void ErilMotion::getWorldTransform(btTransform& centerOfMassWorldTrans) const
{
	if (!m_userPointer->body) return;
	switch (m_userPointer->GetType())
	{
	case 2:
	{
		if (!m_userPointer->moveObject) return;
		Vector loc = m_userPointer->GetMovementTarget()->DesiredState.location;
		Rotator rot = m_userPointer->GetMovementTarget()->DesiredState.rotation;
		btTransform local;
		local.setIdentity();
		local.setOrigin(btVector3(loc.X, loc.Y, loc.Z));
		local.setRotation(btQuaternion((rot.X), (rot.Y), (rot.Z), (rot.W)));
		centerOfMassWorldTrans = local * m_centerOfMassOffset.inverse();
	} break;

	default: 
	{
		Transformation temp = m_userPointer->GetWorldTransformation();
		btTransform local;
		local.setIdentity();
		local.setOrigin(btVector3(temp.Location.X, temp.Location.Y, temp.Location.Z));
		local.setRotation(btQuaternion((temp.Rotation.X), (temp.Rotation.Y), (temp.Rotation.Z), (temp.Rotation.W)));
		centerOfMassWorldTrans = local * m_centerOfMassOffset.inverse();
	} break;
	}
}

void ErilMotion::setWorldTransform(const btTransform& centerOfMassWorldTrans)
{
	if (!m_userPointer->body) return;
	btTransform temp = centerOfMassWorldTrans * m_centerOfMassOffset;

	btVector3 loc = temp.getOrigin();
	Vector wLoc = Vector(loc[0], loc[1], loc[2]) - m_userPointer->GetLocation();

	btQuaternion rot2 = temp.getRotation();
	Rotator rot(rot2[3], rot2[0], rot2[1], rot2[2]);

	btVector3 vel = m_userPointer->body->getLinearVelocity();
	btVector3 gravity = m_userPointer->body->getGravity();

	switch (m_userPointer->GetType())
	{
	case 2: {
		if (!m_userPointer->moveObject) return;
		m_userPointer->GetMovementTarget()->DesiredState.location = wLoc;
		m_userPointer->GetMovementTarget()->DesiredState.velocity = { vel[0], vel[1], vel[2] };
		m_userPointer->GetMovementTarget()->DesiredState.gravity = { gravity[0], gravity[1], gravity[2] };

		m_userPointer->GetMovementTarget()->DesiredState.rotation = rot;
	} break;

	default:
	{
		Transformation local({ loc[0], loc[1], loc[2] }, rot, m_userPointer->GetTarget()->GetScale());
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
	body->setLinearVelocity({ temp.X, temp.Y, temp.Z });
	Vector gra = moveObject->DesiredState.gravity;
	body->setGravity(btVector3(gra.X, gra.Y, gra.Z));
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
	Rotator rot = GetWorldRotation();
	btTransform temp;
	body->getMotionState()->getWorldTransform(temp);
	temp.setOrigin(btVector3(loc.X, loc.Y, loc.Z));
	temp.setRotation(btQuaternion((rot.X), (rot.Y), (rot.Z), (rot.W)));
	body->setWorldTransform(temp);
	Physics::ForceUpdate(body);
}

void ColliderComponent::OnCollide()
{
	Console::Log("collision detected");
}

