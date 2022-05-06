#include "Interface/IRender.h"
#include "ForestPlayer.h"
#include "Objects/MovementComponent.h"
#include "Objects/Terrain.h"
#include "Timer.h"
#include "Objects/InstancedObject.h"
#include "Ghost.h"
#include "Objects/Actor.h"
#include "TestArea/TestUI.h"
#include "PauseUI.h"
#include <Interface/WindowManager.h>
#include <GamePlay/Scene.h>
#include "EndScreen.h"
#include "StartScreen.h"
#include "Objects/InputComponent.h"
#include "Objects/ColliderComponent.h"

//ECS
#include <Interface/IECS.h>
#include <ECS_Examples/ECSExample.h>
#include <ECS/Components/LightComponent.h>
#include <ECS/Systems/LightControllerSystem.h>
#include <ECS/Components/AudioComponent.h>
#include <ECS/Systems/AudioControllerSystem.h>

class Ghost;

void ForestPlayer::OpenConsole(bool) {
	Console::Create();
}

void ForestPlayer::UseCursor(bool keydown)
{
	if (keydown && pause == nullptr) {
		SetShowCursor(cursorState);
		cursorState = !cursorState;
	}
}

void ForestPlayer::Caught()
{
	end = SpawnObject<EndScreen>();
	UI::AddToScreen(end, this);
	end->SetText("You lost!");
	SetShowCursor(true);
	cursorState = false;
	Movement->SetAllowMovement(false);
}

void ForestPlayer::Winner()
{
	end = SpawnObject<EndScreen>();
	UI::AddToScreen(end, this);
	end->SetText("You won!");
	SetShowCursor(true);
	cursorState = false;
	Movement->SetAllowMovement(false);
	ObjectManager::GetByRecord<Ghost>(0x10)->stopMoving();
}

ForestPlayer::ForestPlayer() : Player()
{
	mouseSens = 0.1f;
	Speed = 2.5f;

	InputMode = true;
	cursorState = true;
	pause = nullptr;
	end = nullptr;
	start = nullptr;
	spawnCounter = 0;

	//Reqister used Inputs
	

	Movement = SpawnObject<MovementComponent>();
	Movement->SetTarget(dynamic_cast<Actor*>(this));
	Movement->SetGravity(true);
	Movement->SetPhysics(false);
	Movement->SetMaxSpeed(Speed);

	GetCamera()->SetPostProcess("PostProcessForest");
}

void ForestPlayer::RegisterInputs(InputComponent* com) 
{
	com->RegisterKeyContinuousInput(87, &ForestPlayer::RunInputW, this);
	com->RegisterKeyContinuousInput(65, &ForestPlayer::RunInputA, this);
	com->RegisterKeyContinuousInput(83, &ForestPlayer::RunInputS, this);
	com->RegisterKeyContinuousInput(68, &ForestPlayer::RunInputD, this);
	com->RegisterKeyInput(32, &ForestPlayer::RunInputSpace, this);
	com->RegisterKeyInput(340, &ForestPlayer::RunInputShift, this);
	com->RegisterKeyInput(0, &ForestPlayer::LeftMouseDown, this);
	com->RegisterKeyInput(1, &ForestPlayer::RightMouseDown, this);
	com->RegisterKeyInput(49, &ForestPlayer::InputOne, this);
	com->RegisterKeyInput(50, &ForestPlayer::InputTwo, this);
	com->RegisterKeyInput(256, &ForestPlayer::InputExit, this);
	com->RegisterKeyInput(257, &ForestPlayer::OpenConsole, this);
	com->RegisterMouseInput(0, &ForestPlayer::MouseMoved, this);
	com->RegisterKeyInput(69, &ForestPlayer::ItemPickE, this);
	com->RegisterKeyInput(81, &ForestPlayer::InputQ, this);
}

void ForestPlayer::RunInputW(float delta, bool KeyDown)
{
	Vector dir = -GetCamera()->GetForwardVector();
	dir.Z = 0.f;
	Movement->AddInput(dir.Normalize());
}

void ForestPlayer::RunInputA(float delta, bool KeyDown)
{
	Movement->AddInput(-GetCamera()->GetRightVector());
}

void ForestPlayer::RunInputD(float delta, bool KeyDown)
{
	Movement->AddInput(GetCamera()->GetRightVector());
}

void ForestPlayer::RunInputS(float delta, bool KeyDown)
{
	Vector dir = GetCamera()->GetForwardVector();
	dir.Z = 0.f;
	Movement->AddInput(dir.Normalize());
}

void ForestPlayer::RunInputSpace(bool KeyDown)
{
	if (!Movement->IsInAir() && KeyDown) {
		Movement->AddImpulse(Vector(0.f, 0.f, 200.f));
	}
}

void ForestPlayer::InputOne(bool KeyDown)
{
	
}

void ForestPlayer::InputTwo(bool KeyDown)
{
	
}

void ForestPlayer::RunInputShift(bool KeyDown)
{
	if (KeyDown) {
		Movement->SetMaxSpeed(4.f);
	}
	else {
		Movement->SetMaxSpeed(Speed);
	}
}

void ForestPlayer::LeftMouseDown(bool KeyDown)
{
	//DisableInput();
}

void ForestPlayer::RightMouseDown(bool KeyDown)
{
	/*if (KeyDown == true)
	Console::Log((GetCamera()->GetLocation() + Vector(0.f, 0.f, -2.2f)).ToString());*/
}

void ForestPlayer::InputQ(bool KeyDown)
{
	if (start != nullptr && end == nullptr) {
		start->UI::RemoveFromScreen();
		start = nullptr;
		SetShowCursor(false);
		cursorState = true;
		Movement->SetAllowMovement(true);
		ObjectManager::GetByRecord<Ghost>(0x10)->startMoving();
	}
}

void ForestPlayer::ItemPickE(bool KeyDown)
{
	if (!KeyDown) {

		float distance = 10000.f;
		auto index = Candys.end();
		for (auto i = Candys.begin(); i != Candys.end(); i++) {
			float d = ((*i)->GetLocation() - GetLocation()).Length();
			distance = d;
			index = i;
			if (distance < 2.0f) {
				(*index)->DestroyObject();
				Candys.erase(index);
				Items.push_back(SpawnObject<Item>());
				Console::Log("Items: " + std::to_string(Items.size()));
				if (Items.size() == 5) {
					Winner();
				}
				break;
			}
		}
	}
}

void ForestPlayer::MouseMoved(float X, float Y)
{
	const Rotator& rot = Rotation;
	if (cursorState) SetRotation(Rotator(rot.W, rot.X + X * mouseSens, rot.Y + Y * mouseSens < 89.f && rot.Y + Y * mouseSens > -89.f ? rot.Y + Y * mouseSens : rot.Y, rot.Z));
}

void ForestPlayer::InputExit(bool down)
{
	if (!down) return;
	if (pause == nullptr) {
		pause = SpawnObject<PauseUI>();
		UI::AddToScreen(pause, this);
		SetShowCursor(true);
		cursorState = false;
	}
	else {
		pause->DestroyObject();
		pause = nullptr;
		SetShowCursor(false);
		cursorState = true;
	}
}

void ForestPlayer::Tick(float)
{
	GetCamera()->SetLocation(Location + Vector(0.f, 0.f, 2.2f));
	GetCamera()->SetRotation(Rotation);
}

void ForestPlayer::BeginPlay()
{
	Movement->SetGround(ObjectManager::GetByRecord<Terrain>(0xA0001111));
	ObjectManager::GetByRecord<Ghost>(0x10)->stopMoving();

	Console::Log("Hello beautiful world");

	Candys.push_back(ObjectManager::GetByRecord<VisibleObject>(0xC1));
	Candys.push_back(ObjectManager::GetByRecord<VisibleObject>(0xC2));
	Candys.push_back(ObjectManager::GetByRecord<VisibleObject>(0xC3));
	Candys.push_back(ObjectManager::GetByRecord<VisibleObject>(0xC4));
	Candys.push_back(ObjectManager::GetByRecord<VisibleObject>(0xC5));

	start = SpawnObject<StartScreen>();
	UI::AddToScreen(start, this);
	SetShowCursor(true);
	cursorState = false;
	Movement->SetAllowMovement(false);
}

void ForestPlayer::OnDestroyed()
{

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






