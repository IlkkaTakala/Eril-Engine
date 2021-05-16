#pragma once
#include "Core.h"
#include "Settings.h"
#include "Gameplay/PlayerController.h"

class Actor;

class TestPlayer : public Player
{
public:
	TestPlayer();
	virtual ~TestPlayer() { /*INI->SetValue("Player", "Start", CameraPoint.ToString()); INI->SetValue("Player", "Direction", CameraDirection.ToString());*/ }
	virtual void Tick(float Delta) override;
	virtual void BeginPlay() override;
private:
	float mouseSens;
	float Speed;

	void RunInputW(float delta, bool KeyDown);
	void RunInputA(float delta, bool KeyDown);
	void RunInputD(float delta, bool KeyDown);
	void RunInputS(float delta, bool KeyDown);
	void RunInputSpace(float delta, bool KeyDown);
	void RunInputShift(float delta, bool KeyDown);
	void LeftMouseDown(float delta, bool KeyDown);
	void MouseMoved(float X, float Y);

	void InputExit(float delta, bool) { Exit(); }

	Ref<Actor> Domain;
	std::vector<Ref<VisibleObject>> cubes;
};