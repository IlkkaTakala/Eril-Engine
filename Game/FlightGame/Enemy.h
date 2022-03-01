#pragma once
#include <Core.h>
#include <Objects/Actor.h>

class MovementComponent;

class Enemy : public Actor
{
	REGISTER(Enemy)
public:
	Enemy();
	Ref<MovementComponent> move;

	virtual void Tick(float) override;
	void stopMoving();
	void startMoving();

private:
	Ref<VisibleObject> Mesh;

	float time;
	Vector targetLoc;
	bool caught;

	void SetNewTarget(float);
};

