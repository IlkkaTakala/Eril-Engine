#pragma once
#include "Core.h"
#include "Settings.h"
#include "Gameplay/PlayerController.h"

class Actor;
class InstancedObject;
class Light;

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
	void InputOne(float delta, bool KeyDown);
	void InputTwo(float delta, bool KeyDown);
	void RunInputShift(float delta, bool KeyDown);
	void LeftMouseDown(float delta, bool KeyDown);
	void RightMouseDown(float delta, bool KeyDown);
	void MouseMoved(float X, float Y);

	void InputExit(float delta, bool) { Exit(); }

	Ref<Actor> Domain;
	Ref<Actor> Pillars;

	bool LightMode;
	Ref<Light> DirLight;
	std::vector<Ref<Light>> Lights;
	Ref<InstancedObject> Reflecting;
	std::vector<Ref<Actor>> Spheres;

	Ref<Actor> LastSphere;
};