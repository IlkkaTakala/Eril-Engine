#pragma once
#include "Core.h"
#include "Settings.h"
#include "Gameplay/PlayerController.h"

class Actor;
class InstancedObject;
class MovementComponent;
class Terrain;
class Hunter;
class PauseUI;
class EndScreen;
class StartScreen;
class VisibleObject;
class ScoreUI;

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

class FlightPlayer : public Player
{
	REGISTER(FlightPlayer);
public:
	FlightPlayer();
	virtual ~FlightPlayer() { }
	virtual void Tick(float Delta) override;
	virtual void BeginPlay() override;
	virtual void OnDestroyed() override;
	void Caught();
	void Winner();

	void AddScore();
	void InputQ(bool KeyDown);

private:
	float mouseSens;
	float Speed;

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
	Ref<InstancedObject> Clouds;

	int score;

	Ref<ScoreUI> Scoreui;
	Ref<VisibleObject> Sky;
	PauseUI* pause;
	EndScreen* end;
	StartScreen* start;

	float time;
};