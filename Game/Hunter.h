#pragma once
#include <Core.h>
#include <Objects/Actor.h>

class MovementComponent;

class Hunter : public Actor
{
	REGISTER(Hunter)
public:
	Hunter();
	Ref<MovementComponent> move;

	virtual void Tick(float) override;
private:
	Ref<VisibleObject> Mesh;

	float time;
	Vector targetLoc;
	float targetRot;
	bool caught;

	void SetNewTarget(float);
};

