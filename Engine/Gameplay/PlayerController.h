#pragma once
#include "Core.h"
#include "IRender.h"

class Player : public BaseObject, public Tickable
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

