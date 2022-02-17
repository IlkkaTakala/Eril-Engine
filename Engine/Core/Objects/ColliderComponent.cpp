#include "ColliderComponent.h"
#include <Physics/BulletPhysics.h>
#include <Physics.h>
#include "MovementComponent.h"
#include <btBulletDynamicsCommon.h>

class ErilMotion : public btMotionState
{
	btTransform m_centerOfMassOffset;
	RefWeak<ColliderComponent> m_userPointer;
public:

	ErilMotion(ColliderComponent* c, const btTransform& startTrans = btTransform::getIdentity(), const btTransform& centerOfMassOffset = btTransform::getIdentity()) :
		m_centerOfMassOffset(centerOfMassOffset),
		m_userPointer(c)
	{
	}

	///synchronizes world transform from user to physics
	virtual void getWorldTransform(btTransform& centerOfMassWorldTrans) const
	{
		if (!m_userPointer->body) return;
		switch (m_userPointer->GetType())
		{
		case 2:
		{
			if (!m_userPointer->moveObject) return;
			Vector loc = m_userPointer->GetMovementTarget()->DesiredState.location;
			Vector rot = m_userPointer->GetMovementTarget()->DesiredState.location;
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

	///synchronizes world transform from physics to user
	///Bullet only calls the update of worldtransform for active objects
	virtual void setWorldTransform(const btTransform& centerOfMassWorldTrans)
	{
		if (!m_userPointer->body) return;
		btTransform temp = centerOfMassWorldTrans * m_centerOfMassOffset;
		btVector3 loc = temp.getOrigin();
		btQuaternion rot2 = temp.getRotation();
		Vector rot;
		rot2.getEulerZYX(rot.Y, rot.Z, rot.X);
		rot.X = degrees(rot.X);
		rot.Y = degrees(rot.Y);
		rot.Z = degrees(rot.Z);

		switch (m_userPointer->GetType())
		{
		case 2: {
			if (!m_userPointer->moveObject) return;
			m_userPointer->GetMovementTarget()->DesiredState.location = { loc[0], loc[2], loc[1] };
			m_userPointer->GetMovementTarget()->DesiredState.rotation = rot;
		} break;

		default: 
		{
			Transformation local({ loc[0], loc[2], loc[1] }, rot, m_userPointer->GetTarget()->GetScale());
			m_userPointer->GetTarget()->SetTransform(local);
		} break;
		}
	}
};

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

void ColliderComponent::SetType(int t)
{
	type = t;
	switch (type)
	{
	case 0:
		body = Physics::addBoxStatic(1.0, 1.0, 1.0, 1.0, 1.0, 1.0, new ErilMotion(this));
		break;
	case 1:
		body = Physics::addBox(1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 100, new ErilMotion(this));
		body->setAngularFactor(1.f);
		break;
	case 2:
		body = Physics::addBox(1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 100, new ErilMotion(this));
		body->setActivationState(DISABLE_DEACTIVATION);
		body->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
		body->setAngularFactor(0.f);
		break;
	}
	Refresh();
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
	if (type == 0) {
		body->setWorldTransform(temp);
		Physics::ForceUpdate(body);
	}
	else body->getMotionState()->setWorldTransform(temp);
}

