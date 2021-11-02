#pragma once
#include <Core.h>
#include <Objects/Actor.h>

class MovementComponent;

class Hunter : public Actor
{
public:
	Hunter();
	Ref<MovementComponent> move;

	virtual void Tick(float) override;
private:
	Ref<VisibleObject> Mesh;

	float time;
	Vector targetLoc;
	bool caught;

	void SetNewTarget(float);
};

