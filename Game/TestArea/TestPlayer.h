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

//ECS TEST
class ECSExample;

class TestPlayer : public Player
{
	REGISTER(TestPlayer);
public:
	TestPlayer();
	virtual ~TestPlayer() { }
	virtual void Tick(float Delta) override;
	virtual void BeginPlay() override;
	virtual void OnDestroyed() override;

private:
	float mouseSens;
	float Speed;

	void RunInputQ(float delta, bool KeyDown);
	void RunInputZ(float delta, bool KeyDown);
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
	Ref<VisibleObject> Sky;
	Ref<Actor> Collider;
	Ref<MovementComponent> ColliderModelMove;
	Ref<VisibleObject> ColliderModel;


	Ref<Terrain> terra[4];
	Ref<Hunter> hunt;
	PauseUI* pause;

	//ECS TEST
	Ref<ECSExample> ecsExample;

	int audioComponentID;

	int spawnCounter;

	void TestTimer(float d);
};
