#pragma once
#include "Core.h"
#include "Settings.h"
#include "Gameplay/PlayerController.h"

class Actor;
class MovementComponent;
class Terrain;
class PauseUI;
class CapsuleCollisionShape;
class BoxCollisionShape;
class VisibleObject;
class SkeletalObject;
struct AudioComponent;

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

	Vector GetWalk();
	bool GetGun() const { return gunOut; }

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
	Ref<SkeletalObject> Mesh;

	Ref<SceneComponent> Spring;

	bool cursorState;
	bool InputMode;
	Ref<VisibleObject> Sky;
	Ref<CapsuleCollisionShape> PlayerCol;

	Ref<VisibleObject> Plane;
	Ref<BoxCollisionShape> PlaneCol;

	PauseUI* pause;

	//ECS TEST
	Ref<ECSExample> ecsExample;

	AudioComponent* audio;

	int audioComponentID;

	int spawnCounter;

	float walk;
	bool Changing;
	bool gunOut;

	void TestTimer(float d);
};
