#include "Interface/IRender.h"
#include "TestPlayer.h"
#include "Objects/MovementComponent.h"
#include "Objects/Terrain.h"
#include "Timer.h"
#include "Objects/InstancedObject.h"
#include "Hunter.h"
#include "TestUI.h"
#include "PauseUI.h"
#include <Interface/WindowManager.h>
#include <Interface/AudioManager.h>
#include <GamePlay/Scene.h>

//ECS
#include <Interface/IECS.h>
#include <ECS_Examples/ECSExample.h>
#include <ECS/Components/LightComponent.h>
#include <ECS/Systems/LightControllerSystem.h>
#include <ECS/Components/AudioComponent.h>
#include <ECS/Systems/AudioControllerSystem.h>


void TestPlayer::OpenConsole(bool) {
	Console::Create();
}

void TestPlayer::UseCursor(bool keydown)
{
	if (keydown && pause == nullptr) {
		WindowManager::SetShowCursor(0, cursorState);
		cursorState = !cursorState;
	}
}

TestPlayer::TestPlayer() : Player()
{

	mouseSens = 0.5f;
	Speed = 5.f;
	InputMode = true;
	cursorState = true;
	spawnCounter = 0;

	//Reqister used Inputs
	II->RegisterKeyContinuousInput(81, &TestPlayer::RunInputQ, this);
	II->RegisterKeyContinuousInput(90, &TestPlayer::RunInputZ, this);
	II->RegisterKeyContinuousInput(87, &TestPlayer::RunInputW, this);
	II->RegisterKeyContinuousInput(65, &TestPlayer::RunInputA, this);
	II->RegisterKeyContinuousInput(83, &TestPlayer::RunInputS, this);
	II->RegisterKeyContinuousInput(68, &TestPlayer::RunInputD, this);
	II->RegisterKeyInput(32, &TestPlayer::RunInputSpace, this);
	II->RegisterKeyInput(340, &TestPlayer::RunInputShift, this);
	II->RegisterKeyInput(0, &TestPlayer::LeftMouseDown, this);
	II->RegisterKeyInput(49, &TestPlayer::InputOne, this);
	II->RegisterKeyInput(50, &TestPlayer::InputTwo, this);
	II->RegisterKeyInput(256, &TestPlayer::InputExit, this);
	II->RegisterKeyInput(257, &TestPlayer::OpenConsole, this);
	II->RegisterMouseInput(0, &TestPlayer::MouseMoved, this);
	II->RegisterKeyInput(69, &TestPlayer::UseCursor, this);

	//Player Model
	Mesh = SpawnObject<VisibleObject>();
	Mesh->SetModel("Cube");
	Mesh->GetModel()->SetAABB(AABB(Vector(-1.f), Vector(1.f)));
	
	SetLocation(Vector(0, 0, 2));

	//Player Movement
	Movement = SpawnObject<MovementComponent>();
	Movement->SetTarget(dynamic_cast<Actor*>(this), Mesh->GetModel()->GetAABB());
	Movement->SetGravity(true);

	//Skybox
	Sky = SpawnObject<VisibleObject>();
	Sky->SetModel(MI->LoadData(Sky, "SkySphere"));
	Sky->GetModel()->SetMaterial(0, RI->LoadMaterialByName("Assets/Materials/Sky"));
	Sky->SetScale(Sky->GetScale() * 2.0f);

	//Testing UI
	auto ui = UI::LoadFromFile("Game/TestArea/testingui2.ui");
	UI::AddToScreen(ui, this);

	pause = nullptr;


		
	Collider = SpawnObject<Actor>();

	//Moment Model -> ColliderModel
	ColliderModel = SpawnObject<VisibleObject>();
	ColliderModel->SetModel("Cube");
	ColliderModel->GetModel()->SetAABB(AABB(Vector(-0.5f), Vector(0.5f)));

	Collider->AddComponent(ColliderModel);
	Collider->SetLocation(Vector(20, 2, 1));
	

	ColliderModelMove = SpawnObject<MovementComponent>();
	ColliderModelMove->SetTarget(Collider, ColliderModel->GetModel()->GetAABB());

	Timer::CreateTimer<TestPlayer>(5.0f, &TestPlayer::TestTimer, this, false, false);

}

void TestPlayer::TestTimer(float d)
{
	Console::Log("Location changed");
	Collider->SetLocation(Vector(30, 0, 1), true);
}


//Handle Inputs
void TestPlayer::RunInputQ(float delta, bool KeyDown)
{
	Vector dir(0.0, 0.0, 1.0);
	Movement->AddInput(dir.Normalize());
}
void TestPlayer::RunInputZ(float delta, bool KeyDown)
{
	Vector dir(0.0, 0.0, -1.0);
	Movement->AddInput(dir.Normalize());
}

void TestPlayer::RunInputW(float delta, bool KeyDown)
{
	Vector dir = -GetCamera()->GetForwardVector();
	dir.Z = 0.f;
	Movement->AddInput(dir.Normalize());
}

void TestPlayer::RunInputA(float delta, bool KeyDown)
{
	Movement->AddInput(-GetCamera()->GetRightVector());
}

void TestPlayer::RunInputD(float delta, bool KeyDown)
{
	Movement->AddInput(GetCamera()->GetRightVector());
}

void TestPlayer::RunInputS(float delta, bool KeyDown)
{
	Vector dir = GetCamera()->GetForwardVector();
	dir.Z = 0.f;
	Movement->AddInput(dir.Normalize());
}

void TestPlayer::RunInputSpace(bool KeyDown)
{
	if (!Movement->IsInAir() && KeyDown)
		Movement->AddImpulse(Vector(0.f, 0.f, 300.f));
}

void TestPlayer::InputOne(bool KeyDown)
{
	if (KeyDown)
		InputMode = !InputMode;
}

void TestPlayer::InputTwo(bool KeyDown)
{
	if (KeyDown)
		InputMode = !InputMode;
	Scene::OpenLevel("Assets/Maps/test");
}

void TestPlayer::RunInputShift(bool KeyDown)
{
	if (KeyDown) Movement->SetMaxSpeed(10.f);
	else Movement->SetMaxSpeed(5.f);
}


void TestPlayer::LeftMouseDown(bool)
{
}

void TestPlayer::RightMouseDown(bool KeyDown)
{

}

void TestPlayer::MouseMoved(float X, float Y)
{
	const Vector& rot = Rotation;
	if (cursorState) SetRotation(Vector(rot.X + X * mouseSens, rot.Y + Y * mouseSens < 89.f && rot.Y + Y * mouseSens > -89.f ? rot.Y + Y * mouseSens : rot.Y, rot.Z));
}

void TestPlayer::InputExit(bool down)
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

void TimeFunction (float d)
{
}


void TestPlayer::Tick(float deltaTime)
{
	
	Vector loc = Collider->GetLocation();
	GetCamera()->SetLocation(Location + Vector(0.f, 0.f, 1.5f));
	GetCamera()->SetRotation(Rotation);
	Sky->SetLocation(Location);
	
	Vector listenerPos = Location;
	Vector listenerOrientation = GetCamera()->GetForwardVector();
	AudioManager::SetListener(listenerPos, -GetCamera()->GetForwardVector(), -GetCamera()->GetUpVector());
}

void TestPlayer::BeginPlay()
{

	Terrain* terrain = ObjectManager::GetByRecord<Terrain>(0xA0005554);

	
	//ECS
	SystemsManager* systemsManager = IECS::GetSystemsManager();
	//Audio Testing
	IComponentArrayQuerySystem<AudioComponent>* audioComponentArraySystem = static_cast<IComponentArrayQuerySystem<AudioComponent>*> (systemsManager->GetSystemByName("AudioControllerSystem"));
	AudioComponent* audio = audioComponentArraySystem->AddComponentToSystem();
	
	//AudioControllerSystem* audioControllerSystem = static_cast<AudioControllerSystem*>(systemsManager->GetSystemByName("AudioControllerSystem"));
	//audioComponentID = audio->GetID();

	Vector audioPos = Vector(20.0f, 20.0f, terrain->GetHeight(20.0f, 20.0f) + 1.5f);
	audio->SetSourceID(AudioManager::LoadAudio("clicketi.WAV"));
	audio->SetPosition(audioPos);
	Mesh->SetLocation(audioPos);
	audio->SetGain(1.0f);
	audio->SetPitch(1.0f);
	audio->SetLooping(true);
	audio->SetSourceRelative(false);
	audio->Play();

	//Lights Testing
	IComponentArrayQuerySystem<LightComponent>* lightSystem = static_cast<IComponentArrayQuerySystem<LightComponent>*> (systemsManager->GetSystemByName("LightControllerSystem"));

	if (lightSystem != nullptr)
	{
		LightComponent* DirLight = lightSystem->AddComponentToSystem();
		DirLight->Location = Vector(0.f, 0.f, 1.f);
		DirLight->LightType = LIGHT_DIRECTIONAL;
		DirLight->Size = 3.f;
		DirLight->Intensity = 1.f;
		DirLight->Color = Vector(1.f);
		DirLight->Rotation = Vector(0.5, 0.5, -0.5);

		for (int i = 0; i < 50; i++)
		{
			//Console::Log("Light addded " + std::to_string(i));
			float x = (float)(rand() % 100);
			float y = (float)(rand() % 100);
			//float s = 1.f - rand() / (float)RAND_MAX * 0.7f;

			LightComponent* light = lightSystem->AddComponentToSystem();
			light->Location = Vector(x, y, terrain->GetHeight(x, y));
			light->LightType = LIGHT_POINT;
			light->Size = 5.f;
			light->Intensity = rand() / (float)RAND_MAX * 20.f;
			light->Color = Vector(x, y, 2.5f);
		}
	}
	Console::Log("Hello beautiful world");
}

void TestPlayer::OnDestroyed()
{
}
