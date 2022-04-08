#include "ColliderComponent.h"
#include <Physics/BulletPhysics.h>
#include <Physics.h>
#include "MovementComponent.h"
#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

inline Vector ToEril(const btVector3& in) {
	return { in[0], -in[2], in[1] };
}

inline Rotator ToEril(const btQuaternion& in) {
	return { in[3], in[0], -in[2], in[1] };
}

inline btVector3 ToBullet(const Vector& in) {
	return { in[0], in[2], -in[1] };
}

inline btQuaternion ToBullet(const Rotator& in) {
	return { in[1], in[3], -in[2], in[0] };
}

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
		local.setOrigin(ToBullet(loc));
		local.setRotation(ToBullet(rot));
		centerOfMassWorldTrans = local * m_centerOfMassOffset.inverse();
	} break;

	default: 
	{
		Transformation temp = m_userPointer->GetWorldTransformation();
		btTransform local;
		local.setIdentity();
		local.setOrigin(ToBullet(temp.Location));
		local.setRotation(ToBullet(temp.Rotation));
		centerOfMassWorldTrans = local * m_centerOfMassOffset.inverse();
	} break;
	}
}

void ErilMotion::setWorldTransform(const btTransform& centerOfMassWorldTrans)
{
	if (!m_userPointer->body) return;
	btTransform temp = centerOfMassWorldTrans * m_centerOfMassOffset;

	btVector3 loc = temp.getOrigin();
	Vector wLoc = ToEril(loc) - m_userPointer->GetLocation();

	btQuaternion rot2 = temp.getRotation();
	Rotator rot(ToEril(rot2));
	rot *= -m_userPointer->GetRotation();

	btVector3 vel = m_userPointer->body->getLinearVelocity();
	btVector3 gravity = m_userPointer->body->getGravity();

	switch (m_userPointer->GetType())
	{
	case 2: {
		if (!m_userPointer->moveObject) return;
		m_userPointer->GetMovementTarget()->DesiredState.location = wLoc;
		m_userPointer->GetMovementTarget()->DesiredState.velocity = ToEril(vel);
		m_userPointer->GetMovementTarget()->DesiredState.gravity = ToEril(gravity);

		m_userPointer->GetMovementTarget()->DesiredState.rotation = rot;
	} break;

	default:
	{
		Transformation local(wLoc, rot, m_userPointer->GetTarget()->GetScale());
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
	body->setLinearVelocity(ToBullet(temp));
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
	temp.setOrigin(ToBullet(loc));
	temp.setRotation(ToBullet(rot));
	body->setWorldTransform(temp);
	Physics::ForceUpdate(body);
}

void ColliderComponent::OnCollide()
{
	Console::Log("collision detected");
}

