#pragma once
#include "Core.h"
#include "Interface/IRender.h"
#include "Objects/Actor.h"

class Player : public Actor
{
public:
	Player();
	virtual ~Player() { }
	virtual void Tick(float Delta) override;
	virtual void BeginPlay() override {};
	virtual void LoadWithParameters(const String& args) override;
	virtual void OnDestroyed() override;
	Camera* GetCamera() const { return PlayerCamera; }

	void SetShowCursor(bool show, uint window = 0);
	bool GetShowCursor(uint window = 0);
protected:

private:
	Camera* PlayerCamera;
};

