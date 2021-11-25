#pragma once
#include <Core.h>
#include <Objects/Actor.h>

class Terrain;

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
	Vector angular_v;
	Vector acceleration;
	Vector angular_a;
};

class MovementComponent : public BaseObject, public Tickable
{
	REGISTER(MovementComponent);
public:
	MovementComponent();

	virtual void LoadWithParameters(const String& args) override;
	virtual void OnDestroyed() override;

	virtual void BeginPlay() override {}
	virtual void Tick(float) override;

	void SetTarget(SceneComponent* t);
	void SetGround(Terrain* t);
	SceneComponent* GetTarget() const { return Object; }
	void SetMass(float m) { mass = m; }
	void SetMaxSpeed(float speed) { max_speed = speed; }
	void SetPhysics(bool p) { isPhysics = p; }
	void SetGravity(bool g) { isGravity = g; }
	void SetAllowMovement(bool value) { allowMovement = value; }
	bool GetAllowMovement() const { return allowMovement; }
	void ApplyMovement();

	void AddInput(const Vector& dir);
	void AddImpulse(const Force& f) { forces[force_count++] = f; }
	void AddImpulse(const Vector& d) { Force f; f.Direction = d; forces[force_count++] = f; }
	bool IsInAir() { return inAir; }

	State DesiredState;
	State OldState;

private:

	Ref<SceneComponent> Object;
	RefWeak<Terrain> Terra;

	bool isPhysics;
	bool isGravity;
	bool inAir;
	bool allowMovement;

	float mass;
	float in_acceleration;
	float max_speed;
	float drag;
	float brake;
	float air_control;

	Force forces[16];
	Vector directions[16];
	int direction_count;
	int force_count;
};

