#pragma once
#include "ColliderComponent.h"

class BoxCollisionShape : public ColliderComponent
{
	REGISTER(BoxCollisionShape);
public:
	virtual void LoadWithParameters(const String& args) override;
	virtual void SetType(int t) override;
	void SetSize(AABB s);
};

class SphereCollisionShape : public ColliderComponent
{
	REGISTER(SphereCollisionShape);
public:
	virtual void SetType(int t) override;
	void SetSize(float radius);
};

class CylinderCollisionShape : public ColliderComponent
{
	REGISTER(CylinderCollisionShape);
public:
	virtual void SetType(int t) override;
	void SetSize(AABB s);
};

class ConeCollisionShape : public ColliderComponent
{
	REGISTER(ConeCollisionShape);
public:
	virtual void SetType(int t) override;
	void SetSize(float radius, float height);
};

class CapsuleCollisionShape : public ColliderComponent
{
	REGISTER(CapsuleCollisionShape);
public:
	virtual void SetType(int t) override;
	void SetSize(float radius, float height);
};