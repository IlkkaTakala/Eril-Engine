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
#include "Objects/InputComponent.h"
#include "Objects/ColliderComponent.h"
#include <Objects/ParticleComponent.h>
#include "CloudParticle.h"

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

void TestPlayer::RegisterInputs(InputComponent* com)
{
	com->RegisterKeyContinuousInput(81, &TestPlayer::RunInputQ, this);
	com->RegisterKeyContinuousInput(90, &TestPlayer::RunInputZ, this);
	com->RegisterKeyContinuousInput(87, &TestPlayer::RunInputW, this);
	com->RegisterKeyContinuousInput(65, &TestPlayer::RunInputA, this);
	com->RegisterKeyContinuousInput(83, &TestPlayer::RunInputS, this);
	com->RegisterKeyContinuousInput(68, &TestPlayer::RunInputD, this);
	com->RegisterKeyInput(32, &TestPlayer::RunInputSpace, this);
	com->RegisterKeyInput(340, &TestPlayer::RunInputShift, this);
	com->RegisterKeyInput(0, &TestPlayer::LeftMouseDown, this);
	com->RegisterKeyInput(49, &TestPlayer::InputOne, this);
	com->RegisterKeyInput(50, &TestPlayer::InputTwo, this);
	com->RegisterKeyInput(256, &TestPlayer::InputExit, this);
	com->RegisterKeyInput(257, &TestPlayer::OpenConsole, this);
	com->RegisterMouseInput(0, &TestPlayer::MouseMoved, this);
	com->RegisterKeyInput(69, &TestPlayer::UseCursor, this);
}

TestPlayer::TestPlayer() : Player()
{

	mouseSens = 0.5f;
	Speed = 5.f;
	InputMode = true;
	cursorState = true;
	spawnCounter = 0;
	
	

	//Player Model
	Mesh = SpawnObject<VisibleObject>();
	Mesh->SetModel("Assets/Meshes/Cube");
	Mesh->GetModel()->SetAABB(AABB(Vector(-1.f, -1.f, 0.f), Vector(1.f, 1.f, 2.f)));
	SetLocation(Vector(0, 0, 2));

	//Player Movement
	Movement = SpawnObject<MovementComponent>();
	Movement->SetTarget(dynamic_cast<Actor*>(this), Mesh->GetModel()->GetAABB());
	Movement->SetGravity(true);
	Movement->SetPhysics(false);
	Movement->SetMaxSpeed(Speed);
	Movement->SetFlightMaxSpeed(Speed);
	Movement->SetAirBrake(10.f);
	Movement->SetAcceleration(500.f);
	Movement->SetAirControl(0.9f);

	PlayerCol = SpawnObject<ColliderComponent>();
	AddComponent(PlayerCol);
	PlayerCol->SetLocation(Vector(0.f, 0.f, 1.f), true);
	PlayerCol->SetType(2);
	PlayerCol->SetSize(Mesh->GetModel()->GetAABB());
	PlayerCol->SetMovementTarget(Movement);

	//Skybox
	Sky = SpawnObject<VisibleObject>();
	Sky->SetModel("Assets/Meshes/SkySphere");
	Sky->GetModel()->SetMaterial(0, IRender::LoadMaterialByName("Assets/Materials/Sky"));
	Sky->SetScale(Sky->GetScale() * 2.0f);

	//Testing UI
	/*auto ui = UI::LoadFromFile("Game/TestArea/testingui2.ui");
	UI::AddToScreen(ui, this);*/

	pause = nullptr;

	Plane = SpawnObject<VisibleObject>();
	Plane->SetModel("Assets/Meshes/Cube");
	Plane->GetModel()->SetAABB(AABB(Vector(-20.f, -20.f, -0.5f), Vector(20.f, 20.f, 0.5f)));
	Plane->SetScale(Vector(20.f, 20.f, 0.5f));
	Plane->SetLocation(Vector(10.f, 10.f, 0.f));

	PlaneCol = SpawnObject<ColliderComponent>();
	PlaneCol->SetType(0);
	PlaneCol->SetSize(Plane->GetModel()->GetAABB());
	Plane->AddComponent(PlaneCol);

	Box = SpawnObject<Actor>();

	//Moment Model -> ColliderModel
	BoxModel = SpawnObject<VisibleObject>();
	BoxModel->SetModel("Assets/Meshes/Cube");
	BoxModel->GetModel()->SetAABB(AABB(Vector(-1.0f), Vector(1.0f)));

	Box->AddComponent(BoxModel);
	Box->SetLocation(Vector(10.f, 10.f, 2.f));

	BoxCol = SpawnObject<ColliderComponent>();
	//BoxCol->SetLocation(Box->GetLocation(), true);
	Box->AddComponent(BoxCol);
	BoxCol->SetType(0);
	BoxCol->SetSize(BoxModel->GetModel()->GetAABB());
	
	/*BoxModelMove = SpawnObject<MovementComponent>();
	BoxModelMove->SetTarget(Box, BoxModel->GetModel()->GetAABB());
	BoxModelMove->SetGravity(true);*/
	//BoxModelMove->SetPhysics(true);
	//BoxCol->SetTarget(Box);

	Box2 = SpawnObject<Actor>();

	BoxModel2 = SpawnObject<VisibleObject>();
	BoxModel2->SetModel("Assets/Meshes/Cube");
	BoxModel2->GetModel()->SetAABB(AABB(Vector(-1.0f), Vector(1.0f)));

	Box2->AddComponent(BoxModel2);
	Box2->SetLocation(Vector(10.f, 10.f, 12.f));

	BoxCol2 = SpawnObject<ColliderComponent>();
	Box2->AddComponent(BoxCol2);
	//BoxCol->SetLocation(Box->GetLocation(), true);
	BoxCol2->SetType(1);
	BoxCol2->SetSize(BoxModel2->GetModel()->GetAABB());

	//BoxModelMove = SpawnObject<MovementComponent>();
	//BoxModelMove->SetTarget(Box2, BoxModel2->GetModel()->GetAABB());
	//BoxModelMove->SetGravity(true);
	////BoxModelMove->SetPhysics(true);
	//BoxCol2->SetTarget(BoxModelMove);

	Timer::CreateTimer<TestPlayer>(5.0f, &TestPlayer::TestTimer, this, false, false);

	auto part = SpawnObject<ParticleComponent>();
	part->SetSystem(ParticleSystem::MakeSystem<CloudParticle>());
	part->SetLocation({10.f, 5.f, 0.5f});

}

void TestPlayer::TestTimer(float d)
{
	//Console::Log("Location changed");
	//Collider->SetLocation(Vector(30, 0, 1), true);
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
		Movement->AddImpulse(Vector(0.f, 0.f, 600.f));
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
	if (cursorState) SetRotation(Vector(
		rot.X, 
		rot.Y + Y * mouseSens < 89.f && rot.Y + Y * mouseSens > -89.f ? rot.Y + Y * mouseSens : rot.Y, 
		rot.Z + X * mouseSens));
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
	//Console::Log((BoxCol->GetWorldLocation()).ToString() + " BoxCol");
	//Console::Log((Box->GetWorldLocation()).ToString() + " Box");
	//Console::Log((GetWorldLocation()).ToString() + " Player");
	//Console::Log((PlayerCol->GetWorldLocation()).ToString() + " PlayerCol");

	Vector loc = Box->GetLocation();
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
