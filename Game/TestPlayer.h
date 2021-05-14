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

	void RunInputW(bool KeyDown);
	void RunInputA(bool KeyDown);
	void RunInputD(bool KeyDown);
	void RunInputS(bool KeyDown);
	void RunInputSpace(bool KeyDown);
	void RunInputShift(bool KeyDown);
	void LeftMouseDown(bool KeyDown);
	void MouseMoved(float X, float Y);

	void InputExit(bool) { Exit(); }

	Ref<Actor> Domain;
	std::vector<Ref<VisibleObject>> cubes;
};