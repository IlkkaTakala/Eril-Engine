#include "Interface/IRender.h"
#include "FlightPlayer.h"
#include "Objects/MovementComponent.h"
#include "Objects/Terrain.h"
#include "Timer.h"
#include "Objects/InstancedObject.h"
#include "Enemy.h"
#include "Objects/Actor.h"
#include "TestArea/TestUI.h"
#include "PauseUI.h"
#include <Interface/WindowManager.h>
#include <GamePlay/Scene.h>
#include "EndScreen.h"
#include "StartScreen.h"
#include "Objects/InputComponent.h"
#include "Material.h"

//ECS
#include <Interface/IECS.h>
#include <ECS_Examples/ECSExample.h>
#include <ECS/Components/LightComponent.h>
#include <ECS/Systems/LightControllerSystem.h>
#include <ECS/Components/AudioComponent.h>
#include <ECS/Systems/AudioControllerSystem.h>

class Enemy;

void FlightPlayer::OpenConsole(bool) {
	Console::Create();
}

void FlightPlayer::UseCursor(bool keydown)
{
	if (keydown && pause == nullptr) {
		WindowManager::SetShowCursor(0, cursorState);
		cursorState = !cursorState;
	}
}

void FlightPlayer::Caught()
{
	end = SpawnObject<EndScreen>();
	UI::AddToScreen(end, this);
	end->SetText("You lost!");
	WindowManager::SetShowCursor(0, true);
	cursorState = false;
	Movement->SetAllowMovement(false);
}

void FlightPlayer::Winner()
{
	end = SpawnObject<EndScreen>();
	UI::AddToScreen(end, this);
	end->SetText("You won!");
	WindowManager::SetShowCursor(0, true);
	cursorState = false;
	Movement->SetAllowMovement(false);
	ObjectManager::GetByRecord<Enemy>(0x10)->stopMoving();
}

FlightPlayer::FlightPlayer() : Player()
{
	mouseSens = 0.1f;
	Speed = 2.5f;
	time = 0.f;
	InputMode = true;
	cursorState = true;
	pause = nullptr;
	end = nullptr;
	start = nullptr;

	Movement = SpawnObject<MovementComponent>();
	Movement->SetTarget(dynamic_cast<Actor*>(this));
	Movement->SetGravity(false);
	Movement->SetPhysics(false);
	Movement->SetMaxSpeed(Speed);

	//GetCamera()->SetPostProcess("PostProcessForest");
}

void FlightPlayer::RegisterInputs(InputComponent* com) 
{
	com->RegisterKeyContinuousInput(87, &FlightPlayer::RunInputW, this);
	com->RegisterKeyContinuousInput(65, &FlightPlayer::RunInputA, this);
	com->RegisterKeyContinuousInput(83, &FlightPlayer::RunInputS, this);
	com->RegisterKeyContinuousInput(68, &FlightPlayer::RunInputD, this);
	com->RegisterKeyInput(32, &FlightPlayer::RunInputSpace, this);
	com->RegisterKeyInput(340, &FlightPlayer::RunInputShift, this);
	com->RegisterKeyInput(0, &FlightPlayer::LeftMouseDown, this);
	com->RegisterKeyInput(1, &FlightPlayer::RightMouseDown, this);
	com->RegisterKeyInput(49, &FlightPlayer::InputOne, this);
	com->RegisterKeyInput(50, &FlightPlayer::InputTwo, this);
	com->RegisterKeyInput(256, &FlightPlayer::InputExit, this);
	com->RegisterKeyInput(257, &FlightPlayer::OpenConsole, this);
	com->RegisterMouseInput(0, &FlightPlayer::MouseMoved, this);
	com->RegisterKeyInput(69, &FlightPlayer::ItemPickE, this);
	com->RegisterKeyInput(81, &FlightPlayer::InputQ, this);
}

void FlightPlayer::RunInputW(float delta, bool KeyDown)
{
	Vector dir = -GetCamera()->GetForwardVector();
	dir.Z = 0.f;
	Movement->AddInput(dir.Normalize());
}

void FlightPlayer::RunInputA(float delta, bool KeyDown)
{
	Movement->AddInput(-GetCamera()->GetRightVector());
}

void FlightPlayer::RunInputD(float delta, bool KeyDown)
{
	Movement->AddInput(GetCamera()->GetRightVector());
}

void FlightPlayer::RunInputS(float delta, bool KeyDown)
{
	Vector dir = GetCamera()->GetForwardVector();
	dir.Z = 0.f;
	Movement->AddInput(dir.Normalize());
}

void FlightPlayer::RunInputSpace(bool KeyDown)
{
	if (!Movement->IsInAir() && KeyDown) {
		Movement->AddImpulse(Vector(0.f, 0.f, 200.f));
	}
}

void FlightPlayer::InputOne(bool KeyDown)
{
	
}

void FlightPlayer::InputTwo(bool KeyDown)
{
	
}

void FlightPlayer::RunInputShift(bool KeyDown)
{
	if (KeyDown) {
		Movement->SetMaxSpeed(4.f);
	}
	else {
		Movement->SetMaxSpeed(Speed);
	}
}

void FlightPlayer::LeftMouseDown(bool KeyDown)
{
	//DisableInput();
}

void FlightPlayer::RightMouseDown(bool KeyDown)
{
	/*if (KeyDown == true)
	Console::Log((GetCamera()->GetLocation() + Vector(0.f, 0.f, -2.2f)).ToString());*/
}

void FlightPlayer::InputQ(bool KeyDown)
{
	if (start != nullptr && end == nullptr) {
		start->UI::RemoveFromScreen();
		start = nullptr;
		WindowManager::SetShowCursor(0, false);
		cursorState = true;
		Movement->SetAllowMovement(true);
		//ObjectManager::GetByRecord<Enemy>(0x10)->startMoving();
	}
}

void FlightPlayer::ItemPickE(bool KeyDown)
{
	
}

void FlightPlayer::MouseMoved(float X, float Y)
{
	const Vector& rot = Rotation;
	if (cursorState) SetRotation(Vector(rot.X + X * mouseSens, rot.Y + Y * mouseSens < 89.f && rot.Y + Y * mouseSens > -89.f ? rot.Y + Y * mouseSens : rot.Y, rot.Z));
}

void FlightPlayer::InputExit(bool down)
{
	if (!down) return;
	if (pause == nullptr) {
		pause = SpawnObject<PauseUI>();
		UI::AddToScreen(pause, this);
		WindowManager::SetShowCursor(0, true);
		cursorState = false;
	}
	else {
		pause->DestroyObject();
		pause = nullptr;
		WindowManager::SetShowCursor(0, false);
		cursorState = true;
	}
}

void FlightPlayer::Tick(float delta)
{
	GetCamera()->SetLocation(Location + Vector(0.f, 0.f, 2.2f));
	GetCamera()->SetRotation(Rotation);

	if (Sky) {
		time += delta;
		Sky->SetLocation(Location);
		Sky->GetModel()->GetMaterial(0)->SetParameter("time", time);
	}
}

void FlightPlayer::BeginPlay()
{
	//ObjectManager::GetByRecord<Enemy>(0x10)->stopMoving();
	Sky = ObjectManager::GetByRecord<VisibleObject>(0x5AA);

	Console::Log("Hello beautiful world");

	start = SpawnObject<StartScreen>();
	UI::AddToScreen(start, this);
	WindowManager::SetShowCursor(0, true);
	cursorState = false;
	Movement->SetAllowMovement(false);
}

void FlightPlayer::OnDestroyed()
{
	Player::OnDestroyed();
}

Item::Item() : BaseObject()
{

}

void Item::BeginPlay()
{

}

void Item::DestroyObject()
{
	BaseObject::DestroyObject();
}






