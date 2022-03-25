#include "CollisionShape.h"
#include <Physics/BulletPhysics.h>


void BoxCollisionShape::SetType(int t)
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
		body = Physics::addBox(1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1000, new ErilMotion(this));
		body->setActivationState(DISABLE_DEACTIVATION);
		body->setAngularFactor(0.f);
		body->setFriction(0.f);
		break;
	}
	Refresh();
}

void BoxCollisionShape::SetSize(AABB s)
{
	float width = s.maxs.X - s.mins.X;
	float height = s.maxs.Z - s.mins.Z;
	float depth = s.maxs.Y - s.mins.Y;
	size = s;
	btBoxShape* box = new btBoxShape(btVector3(width / 2.f, height / 2.f, depth / 2.f));
	body->setCollisionShape(box);
}

void SphereCollisionShape::SetType(int t)
{
	type = t;
	switch (type)
	{
	case 0:
		body = Physics::addSphereStatic(1.0, 1.0, 1.0, 1.0, new ErilMotion(this));
		break;
	case 1:
		body = Physics::addSphere(1.0, 1.0, 1.0, 1.0, 100, new ErilMotion(this));
		body->setAngularFactor(1.f);
		break;
	case 2:
		body = Physics::addSphere(1.0, 1.0, 1.0, 1.0, 1000, new ErilMotion(this));
		body->setActivationState(DISABLE_DEACTIVATION);
		body->setAngularFactor(0.f);
		body->setFriction(0.f);
		break;
	}
	Refresh();
}

void SphereCollisionShape::SetSize(float radius)
{
	btSphereShape* sphere = new btSphereShape(radius);
	body->setCollisionShape(sphere);
}

void CylinderCollisionShape::SetType(int t)
{
	type = t;
	switch (type)
	{
	case 0:
		body = Physics::addCylinderStatic(1.0, 1.0, 1.0, 1.0, 1.0, 1.0, new ErilMotion(this));
		break;
	case 1:
		body = Physics::addCylinder(1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 100, new ErilMotion(this));
		body->setAngularFactor(1.f);
		break;
	case 2:
		body = Physics::addCylinder(1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1000, new ErilMotion(this));
		body->setActivationState(DISABLE_DEACTIVATION);
		body->setAngularFactor(0.f);
		body->setFriction(0.f);
		break;
	}
	Refresh();
}

void CylinderCollisionShape::SetSize(AABB s)
{
	float width = s.maxs.X - s.mins.X;
	float height = s.maxs.Z - s.mins.Z;
	float depth = s.maxs.Y - s.mins.Y;
	size = s;
	btCylinderShape* cylinder = new btCylinderShape(btVector3(width / 2.f, height / 2.f, depth / 2.f));
	body->setCollisionShape(cylinder);
}

void ConeCollisionShape::SetType(int t)
{
	type = t;
	switch (type)
	{
	case 0:
		body = Physics::addConeStatic(1.0, 1.0, 1.0, 1.0, 1.0, new ErilMotion(this));
		break;
	case 1:
		body = Physics::addCone(1.0, 1.0, 1.0, 1.0, 1.0, 100, new ErilMotion(this));
		body->setAngularFactor(1.f);
		break;
	case 2:
		body = Physics::addCone(1.0, 1.0, 1.0, 1.0, 1.0, 1000, new ErilMotion(this));
		body->setActivationState(DISABLE_DEACTIVATION);
		body->setAngularFactor(0.f);
		body->setFriction(0.f);
		break;
	}
	Refresh();
}

void ConeCollisionShape::SetSize(float radius, float height)
{
	btConeShape* cone = new btConeShape(radius, height);
	body->setCollisionShape(cone);
}

void CapsuleCollisionShape::SetType(int t)
{
	type = t;
	switch (type)
	{
	case 0:
		body = Physics::addCapsuleStatic(1.0, 1.0, 1.0, 1.0, 1.0, new ErilMotion(this));
		break;
	case 1:
		body = Physics::addCapsule(1.0, 1.0, 1.0, 1.0, 1.0, 100, new ErilMotion(this));
		body->setAngularFactor(1.f);
		break;
	case 2:
		body = Physics::addCapsule(1.0, 1.0, 1.0, 1.0, 1.0, 1000, new ErilMotion(this));
		body->setActivationState(DISABLE_DEACTIVATION);
		body->setAngularFactor(0.f);
		body->setFriction(0.f);
		break;
	}
	Refresh();
}

void CapsuleCollisionShape::SetSize(float radius, float height)
{
	btCapsuleShape* capsule = new btCapsuleShape(radius, height);
	body->setCollisionShape(capsule);
}
