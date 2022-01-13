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
class PauseUI;
class EndScreen;
class StartScreen;

class Item : public BaseObject
{
	REGISTER(Item)
public:
	Item();

	virtual ~Item() { }

	virtual void BeginPlay();

	virtual void DestroyObject();
};


//ECS TEST
class ECSExample;

class ForestPlayer : public Player
{
	REGISTER(ForestPlayer);
public:
	ForestPlayer();
	virtual ~ForestPlayer() { /*INI->SetValue("Player", "Start", CameraPoint.ToString()); INI->SetValue("Player", "Direction", CameraDirection.ToString());*/ }
	virtual void Tick(float Delta) override;
	virtual void BeginPlay() override;
	virtual void OnDestroyed() override;
	void Caught();
	void Winner();

private:
	float mouseSens;
	float Speed;

	void InputQ(bool KeyDown);
	void ItemPickE(bool KeyDown);
	void RunInputW(float delta, bool KeyDown);
	void RunInputA(float delta, bool KeyDown);
	void RunInputD(float delta, bool KeyDown);
	void RunInputS(float delta, bool KeyDown);
	void RunInputSpace(bool KeyDown);
	void InputOne(bool KeyDown);
	void InputTwo(bool KeyDown);
	void RunInputShift(bool KeyDown);
	void LeftMouseDown(bool KeyDown);
	void RightMouseDown(bool KeyDown);
	void MouseMoved(float X, float Y);

	void InputExit(bool);
	void OpenConsole(bool);
	void UseCursor(bool);
	void RegisterInputs(InputComponent* com);

	Ref<MovementComponent> Movement;
	Ref<VisibleObject> Mesh;

	bool cursorState;
	bool InputMode;
	Ref<Light> DirLight;
	Ref<Light> Lights[100];
	Ref<InstancedObject> Trees;
	Ref<InstancedObject> Trees2;
	Ref<InstancedObject> Grass;
	Ref<InstancedObject> Flowers;
	Ref<InstancedObject> Rocks;
	Ref<InstancedObject> Shacks;
	Ref<InstancedObject> Candy;

	Ref<VisibleObject> Sky;
	Ref<Terrain> terra[4];
	Ref<Hunter> hunt;
	PauseUI* pause;
	EndScreen* end;
	StartScreen* start;
	std::vector<RefWeak<VisibleObject>> Candys;
	std::vector<Ref<Item>> Items;

	//ECS TEST
	Ref<ECSExample> ecsExample;
	int spawnCounter;
};