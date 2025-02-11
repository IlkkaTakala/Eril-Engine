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
#include "Objects/CollisionShape.h"
#include <Objects/ParticleComponent.h>
#include "CloudParticle.h"
#include <Objects/SkeletalObject.h>
#include <Interface/AssetManager.h>

//ECS
#include <Interface/IECS.h>
#include <ECS/Components/LightComponent.h>
#include <ECS/Systems/LightControllerSystem.h>
#include <ECS/Components/AudioComponent.h>
#include <ECS/Systems/AudioControllerSystem.h>

#ifdef USE_SCRIPTCORE
#include <ScriptCore.h>
#endif
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
	com->RegisterKeyInput(1, &TestPlayer::RightMouseDown, this);
}

TestPlayer::TestPlayer() : Player()
{

	mouseSens = 0.5f;
	Speed = 2.f;
	InputMode = true;
	cursorState = true;
	spawnCounter = 0;
	Changing = false;
	walk = 0.f;
	gunOut = false;
	
	Rotation = Rotator(0.f);
	SetLocation(Vector(2, 0, 1));

	//Player Model
	Mesh = SpawnObject<SkeletalObject>(this);

	Mesh->SetModel("Assets/Skeletal/Alien");
	Mesh->GetModel()->SetMaterial(1, IRender::LoadMaterialByName("Assets/Materials/alien_upper"));
	Mesh->GetModel()->SetMaterial(0, IRender::LoadMaterialByName("Assets/Materials/alien_lower"));
	Mesh->GetModel()->SetAABB(AABB(Vector(-1.f, -1.f, 0.f), Vector(1.f, 1.f, 2.f)));
	Mesh->SetRotation(-180);
	auto animC = SpawnObject<TestAnimControl>(this, Mesh);
	animC->SetSkeleton(Mesh->GetModel());
	Mesh->SetAnimController(animC);

	Spring = SpawnObject<SceneComponent>(this);
	AddComponent(Spring);

	GetCamera()->SetParent(Spring);
	Spring->SetLocation({0.6f, 0.f, 1.5f});
	GetCamera()->SetLocation({0.f, -2.5f, 0.f});

	AddComponent(Mesh);

	Mesh->SetScale(Vector(0.01f));

	//Player Movement
	Movement = SpawnObject<MovementComponent>(this);
	Movement->SetTarget(dynamic_cast<Actor*>(this), Mesh->GetModel()->GetAABB());
	Movement->SetGravity(true);
	Movement->SetPhysics(false);
	Movement->SetMaxSpeed(Speed);
	Movement->SetFlightMaxSpeed(10000.f);
	Movement->SetAirBrake(2000.f);
	Movement->SetAcceleration(50.f);
	Movement->SetAirControl(0.2f);

	PlayerCol = SpawnObject<CapsuleCollisionShape>(this);
	AddComponent(PlayerCol);
	PlayerCol->SetLocation(Vector(0.f, 0.f, 1.f), true);
	PlayerCol->SetType(2);
	PlayerCol->SetSize(0.5, 1.f);
	PlayerCol->SetMovementTarget(Movement);

	//Skybox
	Sky = SpawnObject<VisibleObject>(this);
	Sky->SetModel("Assets/Meshes/SkySphere");
	Sky->GetModel()->SetMaterial(0, IRender::LoadMaterialByName("Assets/Materials/Sky"));
	Sky->SetScale(Sky->GetScale() * 4.0f);

	pause = nullptr;

	/*Plane = SpawnObject<VisibleObject>();
	Plane->SetModel("Cube");
	Plane->GetModel()->SetAABB(AABB(Vector(-20.f, -20.f, -0.5f), Vector(20.f, 20.f, 0.5f)));
	Plane->SetScale(Vector(20.f, 20.f, 0.5f));
	Plane->SetLocation(Vector(10.f, 10.f, 0.f));

	PlaneCol = SpawnObject<BoxCollisionShape>(this);
	PlaneCol->SetType(0);
	PlaneCol->SetSize(Plane->GetModel()->GetAABB());
	Plane->AddComponent(PlaneCol);*/

	Timer::CreateTimer<TestPlayer>(5.0f, &TestPlayer::TestTimer, this, false, false);

	auto Particle = SpawnObject<ParticleComponent>(this);
	Particle->SetSystem(ParticleSystem::MakeSystem<CloudParticle>());
	Particle->SetLocation(Vector(-116.f, -104.f, 44.f));

	auto Particle2 = SpawnObject<ParticleComponent>(this);
	Particle2->SetSystem(ParticleSystem::MakeSystem<CloudParticle>());
	Particle2->SetLocation(Vector(-60.f, -94.f, 44.f));

	auto smoke = SpawnObject<ParticleComponent>(this);
	smoke->SetSystem(ParticleSystem::MakeSystem<SmokeParticle>());
	smoke->SetLocation(Vector(71.f, -40.f, 11.f));

	auto part = SpawnObject<ParticleComponent>(this);
	part->SetSystem(ParticleSystem::MakeSystem<CloudParticle>());
	part->SetLocation({ 10.f, 5.f, 0.5f });
}

void TestPlayer::TestTimer(float d)
{
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
	Vector dir = GetCamera()->GetForwardVector();
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
	Vector dir = -GetCamera()->GetForwardVector();
	dir.Z = 0.f;
	Movement->AddInput(dir.Normalize());
}

void TestPlayer::RunInputSpace(bool KeyDown)
{
	if (!Movement->IsInAir() && KeyDown)
		Movement->AddImpulse(Vector(0.f, 0.f, 6000.f));
}

void TestPlayer::InputOne(bool KeyDown)
{
	if (!KeyDown) {
		gunOut = !gunOut;
	}
}

void TestPlayer::InputTwo(bool KeyDown)
{
	if (KeyDown)
		Scene::OpenLevel("Assets/Maps/test");
	
}

void TestPlayer::RunInputShift(bool KeyDown)
{
	if (KeyDown) Movement->SetMaxSpeed(6.f);
	else Movement->SetMaxSpeed(Speed);
}


void TestPlayer::LeftMouseDown(bool keydown)
{
	if (keydown && gunOut) {
		auto p = SpawnObject<VisibleObject>(this);
		p->SetModel("Assets/Meshes/tracer");
		p->GetModel()->SetMaterial(0, IRender::LoadMaterialByName("Assets/Materials/tracer"));
		p->SetLocation(Location + Vector{ 0.f, 0.f, 1.3f });
		p->SetRotation(Spring->GetWorldRotation());
		p->SetLifetime(0.5f);
		audio->Play();
	}
}

void TestPlayer::RightMouseDown(bool KeyDown)
{
	Changing = KeyDown;
}

void TestPlayer::MouseMoved(float X, float Y)
{
	if (cursorState && !Changing) {
		Camera* cam = GetCamera();
		Rotator rot = Spring->GetRotation();
		float y = rot.PitchDegrees();
		Spring->SetRotation(Vector(
			rot.RollDegrees(),
			y + Y * mouseSens < 89.f && y + Y * mouseSens > -89.f ? y + Y * mouseSens : y,
			rot.YawDegrees()
		));
		SetRotation(Vector(
			0.f, 
			0.f,
			Rotation.YawDegrees() + X * mouseSens
		));
	}
	if (Changing) {
		walk += X * 0.001f;
	}
}

void TestPlayer::InputExit(bool down)
{
	if (!down) return;
	if (pause == nullptr) {
		pause = SpawnObject<PauseUI>(this);
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
	Sky->SetLocation(Location);
	
	Vector listenerPos = Location;
	Vector listenerOrientation = GetCamera()->GetForwardVector();
	AudioManager::SetListener(listenerPos, GetCamera()->GetForwardVector(), GetCamera()->GetUpVector());

}

void TestPlayer::BeginPlay()
{
	Player::BeginPlay();


	//ECS
	SystemsManager* systemsManager = IECS::GetSystemsManager();
	//Audio Testing
	IComponentArrayQuerySystem<AudioComponent>* audioComponentArraySystem = static_cast<IComponentArrayQuerySystem<AudioComponent>*> (systemsManager->GetSystemByName("AudioControllerSystem"));
	audio = audioComponentArraySystem->AddComponentToSystem();
	
	Vector audioPos = Vector(20.0f, 20.0f, 1.5f);
	audio->SetSourceID(AudioManager::LoadAudio("gun.WAV"));
	audio->SetPosition(audioPos);
	audio->SetGain(1.0f);
	audio->SetPitch(1.0f);
	audio->SetLooping(false);
	audio->SetSourceRelative(false);

	//Lights Testing
	IComponentArrayQuerySystem<LightComponent>* lightSystem = static_cast<IComponentArrayQuerySystem<LightComponent>*> (systemsManager->GetSystemByName("LightControllerSystem"));

	if (lightSystem != nullptr)
	{
		LightComponent* DirLight = lightSystem->AddComponentToSystem();
		DirLight->Location = Vector(0.f, 0.f, 0.f);
		DirLight->LightType = LIGHT_DIRECTIONAL;
		DirLight->Size = 3.f;
		DirLight->Intensity = 2.f;
		DirLight->Color = Vector(1.f);
		DirLight->Rotation = Vector(0.5f, 0.5f, -0.5f).Normalize();

	}
	Console::Log("Hello beautiful world");

	auto hunt = SpawnObject<Hunter>(this);
	hunt->SetLocation({10, 20, 1});

	hunt = SpawnObject<Hunter>(this);
	hunt->SetLocation({ 20, 20, 1 });
}

void TestPlayer::OnDestroyed()
{
}

Vector TestPlayer::GetWalk()
{
	Vector w = {
		Vector::Dot(Rotation.GetRightVector(), Movement->DesiredState.velocity),
		Vector::Dot(Rotation.GetForwardVector(), Movement->DesiredState.velocity),
		0.f
	};
	return w;
}