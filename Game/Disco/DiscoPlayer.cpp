#include "Interface/IRender.h"
#include "DiscoPlayer.h"
#include "Objects/MovementComponent.h"
#include "Objects/Terrain.h"
#include "Timer.h"
#include "Objects/InstancedObject.h"
#include "PauseUI.h"
#include <Interface/WindowManager.h>
#include <GamePlay/Scene.h>

//ECS
#include <Interface/IECS.h>
#include <ECS_Examples/ECSExample.h>
#include <ECS/Components/LightComponent.h>
#include <ECS/Systems/LightControllerSystem.h>


void DiscoPlayer::OpenConsole(bool) {
	Console::Create();
}

void DiscoPlayer::UseCursor(bool keydown)
{
	if (keydown && pause == nullptr) {
		WindowManager::SetShowCursor(0, cursorState);
		cursorState = !cursorState;
	}
}

DiscoPlayer::DiscoPlayer() : Player()
{
	mouseSens = 0.5f;
	Speed = 5.f;
	InputMode = true;
	cursorState = true;
	spawnCounter = 0;

	//Reqister used Inputs
	II->RegisterKeyContinuousInput(81, &DiscoPlayer::RunInputQ, this);
	II->RegisterKeyContinuousInput(90, &DiscoPlayer::RunInputZ, this);
	II->RegisterKeyContinuousInput(87, &DiscoPlayer::RunInputW, this);
	II->RegisterKeyContinuousInput(65, &DiscoPlayer::RunInputA, this);
	II->RegisterKeyContinuousInput(83, &DiscoPlayer::RunInputS, this);
	II->RegisterKeyContinuousInput(68, &DiscoPlayer::RunInputD, this);
	II->RegisterKeyInput(32, &DiscoPlayer::RunInputSpace, this);
	II->RegisterKeyInput(340, &DiscoPlayer::RunInputShift, this);
	II->RegisterKeyInput(0, &DiscoPlayer::LeftMouseDown, this);
	II->RegisterKeyInput(49, &DiscoPlayer::InputOne, this);
	II->RegisterKeyInput(50, &DiscoPlayer::InputTwo, this);
	II->RegisterKeyInput(256, &DiscoPlayer::InputExit, this);
	II->RegisterKeyInput(257, &DiscoPlayer::OpenConsole, this);
	II->RegisterMouseInput(0, &DiscoPlayer::MouseMoved, this);
	II->RegisterKeyInput(69, &DiscoPlayer::UseCursor, this);

	//Player Movement
	Movement = SpawnObject<MovementComponent>();
	Movement->SetTarget(dynamic_cast<Actor*>(this));
	Movement->SetGravity(true);

	pause = nullptr;
}


//Handle Inputs
void DiscoPlayer::RunInputQ(float delta, bool KeyDown)
{
	Vector dir(0.0,0.0,1.0);
	Movement->AddInput(dir.Normalize());
}
void DiscoPlayer::RunInputZ(float delta, bool KeyDown)
{
	Vector dir(0.0, 0.0, -1.0);
	Movement->AddInput(dir.Normalize());
}

void DiscoPlayer::RunInputW(float delta, bool KeyDown)
{
	Vector dir = -GetCamera()->GetForwardVector();
	dir.Z = 0.f;
	Movement->AddInput(dir.Normalize());
}

void DiscoPlayer::RunInputA(float delta, bool KeyDown)
{
	Movement->AddInput(-GetCamera()->GetRightVector());
}

void DiscoPlayer::RunInputD(float delta, bool KeyDown)
{
	Movement->AddInput(GetCamera()->GetRightVector());
}

void DiscoPlayer::RunInputS(float delta, bool KeyDown)
{
	Vector dir = GetCamera()->GetForwardVector();
	dir.Z = 0.f;
	Movement->AddInput(dir.Normalize());
}

void DiscoPlayer::RunInputSpace(bool KeyDown)
{
	if (!Movement->IsInAir() && KeyDown)
		Movement->AddImpulse(Vector(0.f, 0.f, 300.f));
}

void DiscoPlayer::InputOne(bool KeyDown)
{
	if (KeyDown)
		InputMode = !InputMode;
}

void DiscoPlayer::InputTwo(bool KeyDown)
{
	if (KeyDown)
		InputMode = !InputMode;
	Scene::OpenLevel("Assets/Maps/test");
}

void DiscoPlayer::RunInputShift(bool KeyDown)
{
	if (KeyDown) Movement->SetMaxSpeed(10.f);
	else Movement->SetMaxSpeed(5.f);
}


void DiscoPlayer::LeftMouseDown(bool)
{
}

void DiscoPlayer::RightMouseDown(bool KeyDown)
{

}

void DiscoPlayer::MouseMoved(float X, float Y)
{
	const Vector& rot = Rotation;
	if (cursorState) SetRotation(Vector(rot.X + X * mouseSens, rot.Y + Y * mouseSens < 89.f && rot.Y + Y * mouseSens > -89.f ? rot.Y + Y * mouseSens : rot.Y, rot.Z));
}

void DiscoPlayer::InputExit(bool down)
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

void DiscoPlayer::Tick(float)
{
	GetCamera()->SetLocation(Location + Vector(0.f, 0.f, 1.5f));
	GetCamera()->SetRotation(Rotation);
}

void DiscoPlayer::BeginPlay()
{
	//Lights Testing
	SystemsManager* systemsManager = IECS::GetSystemsManager();
	IComponentArrayQuerySystem<LightComponent>* lightSystem = static_cast<IComponentArrayQuerySystem<LightComponent>*> (systemsManager->GetSystemByName("LightControllerSystem"));

	if (lightSystem != nullptr)
	{
		for (int i = 0; i < 20; i++)
		{
			//Console::Log("Light addded " + std::to_string(i));
			int x = rand() % 44;
			int y = rand() % 86;
			int z = rand() % 30;

			float r = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)); // rand between 0-1
			float g = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)); // rand between 0-1
			float b = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)); // rand between 0-1

			//float s = 1.f - rand() / (float)RAND_MAX * 0.7f;

			LightComponent* light = lightSystem->AddComponentToSystem();
			light->Location = Vector(-22 + x, -43 + y, 1 + z);
			light->LightType = LIGHT_POINT;
			light->Size = 15.f;
			light->Intensity = rand() / (float)RAND_MAX * 10.f + 10.0f;
			light->Color = Vector(r, g, b);
		}
	}

	Console::Log("Hello beautiful world");
}

void DiscoPlayer::OnDestroyed()
{
}
