#pragma once
#include <Core.h>
#include <Objects/Actor.h>

struct Force
{
	Vector Location;
	Vector Direction;
	float duration;
};

struct State
{
	Vector location;
	Vector rotation;
	Vector velocity;
	Vector angular;
	Vector acceleration;
	Vector angular_a;
	Vector gravity;
};

class MovementComponent : public BaseObject, public Tickable
{
public:
	MovementComponent();

	virtual void OnDestroyed() override;

	virtual void BeginPlay() override {}
	virtual void Tick(float) override;

	void SetTarget(Actor* t);
	Actor* GetTarget() const { return Object; }
	void SetMass(float m) { mass = m; }
	void SetMaxSpeed(float speed) { max_speed = speed; }
	void SetPhysics(bool p) { isPhysics = p; }
	void SetGravity(bool g) { isGravity = g; }

	void AddInput(const Vector dir) { directions[direction_count++] = dir; }

	State DesiredState;
	State OldState;

private:

	Ref<Actor> Object;

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

