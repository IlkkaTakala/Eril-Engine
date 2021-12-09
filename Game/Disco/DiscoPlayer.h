#pragma once
#include "Core.h"
#include "Settings.h"
#include "Gameplay/PlayerController.h"

class Light;
class MovementComponent;
class PauseUI;

//ECS TEST
class ECSExample;

class DiscoPlayer : public Player
{
	REGISTER(DiscoPlayer);
public:
	DiscoPlayer();
	virtual ~DiscoPlayer() { }
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
	PauseUI* pause;

	std::map<int, std::tuple<float, float>>lightposition;

	//ECS TEST
	Ref<ECSExample> ecsExample;

	int spawnCounter;
};