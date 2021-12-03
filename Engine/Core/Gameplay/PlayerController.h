#pragma once
#include "Core.h"
#include "Interface/IRender.h"
#include "Objects/Actor.h"

class Player : public Actor
{
public:
	Player();
	virtual ~Player() { delete PlayerCamera; }
	virtual void Tick(float Delta) override;
	virtual void BeginPlay() override {};
	virtual void LoadWithParameters(const String& args) override;

	Camera* GetCamera() const { return PlayerCamera; }

protected:

private:
	Camera* PlayerCamera;
};

