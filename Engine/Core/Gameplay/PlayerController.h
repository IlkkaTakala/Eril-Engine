#pragma once
#include "Core.h"
#include "IRender.h"
#include "Objects/Actor.h"

class Player : public Actor
{
public:
	Player();
	virtual ~Player() { delete PlayerCamera; }
	virtual void Tick(float Delta) override;
	virtual void BeginPlay() override {};

	Camera* GetCamera() const { return PlayerCamera; }

protected:

private:
	Camera* PlayerCamera;
};

