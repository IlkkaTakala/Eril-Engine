#pragma once
#include <Core.h>

class Actor;

struct Force
{
	Vector Location;
	Vector Direction;
	float duration;
};

class MovementComponent : public BaseObject, public Tickable
{
public:
	MovementComponent();

	virtual void BeginPlay() override {}
	virtual void Tick(float) override;

	void SetTarget(Actor* t) { Object = t; }
	void SetMass(float m) { mass = m; }
	void SetMaxSpeed(float speed) { max_speed = speed; }
	void SetPhysics(bool p) { isPhysics = p; }
	void SetGravity(bool g) { isGravity = g; }

	void AddInput(const Vector dir) { directions[direction_count++] = dir; }

private:

	Ref<Actor> Object;

	Vector velocity;
	Vector acceleration;
	Vector gravity;

	bool isPhysics;
	bool isGravity;

	float mass;
	float in_acceleration;
	float max_speed;
	float drag;
	float brake;

	Force forces[16];
	Vector directions[16];
	int direction_count;
};

