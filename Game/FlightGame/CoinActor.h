#pragma once
#include <Objects/Actor.h>

class VisibleObject;
class ColliderComponent;

class CoinActor : public Actor
{
public:
	CoinActor();
	virtual void Tick(float delta) override;

private:

	Ref<VisibleObject> mesh;
	Ref<ColliderComponent> collider;
};

