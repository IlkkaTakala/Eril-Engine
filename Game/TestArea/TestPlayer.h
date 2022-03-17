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
class BoxCollisionShape;
class SphereCollisionShape;
class CylinderCollisionShape;
class ConeCollisionShape;
class CapsuleCollisionShape;
class VisibleObject;

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
	void RegisterInputs(InputComponent* com);

	Ref<MovementComponent> Movement;
	Ref<VisibleObject> Mesh;

	bool cursorState;
	bool InputMode;
	Ref<Light> DirLight;
	Ref<VisibleObject> Sky;
	Ref<Actor> Box;
	Ref<Actor> Box2;
	Ref<MovementComponent> BoxModelMove;
	Ref<VisibleObject> BoxModel;
	Ref<BoxCollisionShape> BoxCol;
	Ref<VisibleObject> BoxModel2;
	Ref<CapsuleCollisionShape> BoxCol2;
	Ref<CapsuleCollisionShape> PlayerCol;

	Ref<VisibleObject> Plane;
	Ref<BoxCollisionShape> PlaneCol;

	PauseUI* pause;

	//ECS TEST
	Ref<ECSExample> ecsExample;

	int audioComponentID;

	int spawnCounter;

	void TestTimer(float d);
};
