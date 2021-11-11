#pragma once
#include "Core.h"
#include "Settings.h"
#include "Gameplay/PlayerController.h"

class Actor;
class InstancedObject;
class Light;
class MovementComponent;
class Terrain;
class Hunter;

class Item : public BaseObject
{
	REGISTER(Item)
public:
	Item();

	virtual ~Item() { }

	virtual void BeginPlay();

	virtual void DestroyObject();
};


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

	void ItemPickE(float delta, bool KeyDown);
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

	Ref<MovementComponent> Movement;
	Ref<VisibleObject> Mesh;

	bool InputMode;
	Ref<Light> DirLight;
	Ref<Light> Lights[100];
	Ref<InstancedObject> Trees;
	Ref<InstancedObject> Trees2;
	Ref<InstancedObject> Grass;
	Ref<InstancedObject> Flowers;
	Ref<VisibleObject> Sky;

	Ref<Terrain> terra[4];

	Ref<Hunter> hunt;

	int spawnCounter;
	std::vector<Ref<Item>> items;
};