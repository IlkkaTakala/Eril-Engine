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
#include <ECS_Examples/ECSExample.h>
#include <ECS/Components/LightComponent.h>
#include <ECS/Systems/LightControllerSystem.h>
#include <ECS/Components/AudioComponent.h>
#include <ECS/Systems/AudioControllerSystem.h>

#include <ScriptCore.h>

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
	Speed = 12.f;
	InputMode = true;
	cursorState = true;
	spawnCounter = 0;
	Changing = false;
	walk = 0.f;
	
	Rotation = Rotator(0.f);

	//Player Model
	Mesh = SpawnObject<VisibleObject>();
	Mesh->SetModel("Assets/Meshes/Cube");
	Mesh->GetModel()->SetAABB(AABB(Vector(-1.f, -1.f, 0.f), Vector(1.f, 1.f, 2.f)));
	SetLocation(Vector(15, 15, 1));

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

	PlayerCol = SpawnObject<CapsuleCollisionShape>();
	AddComponent(PlayerCol);
	PlayerCol->SetLocation(Vector(0.f, 0.f, 1.f), true);
	PlayerCol->SetType(2);
	PlayerCol->SetSize(0.5, 1.f);
	PlayerCol->SetMovementTarget(Movement);

	//Skybox
	Sky = SpawnObject<VisibleObject>();
	Sky->SetModel("Assets/Meshes/SkySphere");
	Sky->GetModel()->SetMaterial(0, IRender::LoadMaterialByName("Assets/Materials/Sky"));
	Sky->SetScale(Sky->GetScale() * 4.0f);

	pause = nullptr;

	/*Plane = SpawnObject<VisibleObject>();
	Plane->SetModel("Cube");
	Plane->GetModel()->SetAABB(AABB(Vector(-20.f, -20.f, -0.5f), Vector(20.f, 20.f, 0.5f)));
	Plane->SetScale(Vector(20.f, 20.f, 0.5f));
	Plane->SetLocation(Vector(10.f, 10.f, 0.f));

	PlaneCol = SpawnObject<BoxCollisionShape>();
	PlaneCol->SetType(0);
	PlaneCol->SetSize(Plane->GetModel()->GetAABB());
	Plane->AddComponent(PlaneCol);*/

	Box = SpawnObject<Actor>();

	//Moment Model -> ColliderModel
	/*BoxModel = SpawnObject<VisibleObject>();
	BoxModel->SetModel("Assets/Meshes/Cube");
	BoxModel->GetModel()->SetAABB(AABB(Vector(-1.0f), Vector(1.0f)));

	Box->AddComponent(BoxModel);
	Box->SetLocation(Vector(10.f, 10.f, 2.f));

	BoxCol = SpawnObject<BoxCollisionShape>();
	Box->AddComponent(BoxCol);
	BoxCol->SetType(0);
	BoxCol->SetSize(BoxModel->GetModel()->GetAABB());*/
	

	//Timer::CreateTimer<TestPlayer>(5.0f, &TestPlayer::TestTimer, this, false, false);

	auto skel = SpawnObject<SkeletalObject>();
	skel->SetModel("Assets/Skeletal/Alien");
	skel->GetModel()->SetMaterial(1, IRender::LoadMaterialByName("Assets/Materials/alien_upper"));
	skel->GetModel()->SetMaterial(0, IRender::LoadMaterialByName("Assets/Materials/alien_lower"));
	skel->SetLocation({5.f, 5.f, -0.5f});
	auto animC = SpawnObject<TestAnimControl>(skel);
	animC->BeginPlay();
	animC->SetSkeleton(skel->GetModel());
	skel->SetAnimController(animC);
	//skel->SetParent(this);

	//animC->SetOverrideAnimation(AssetManager::LoadAnimationAsyncWithPromise("Assets/Animations/Breakdance", skel->GetModel()));
	skel->SetScale(Vector(0.01f));

	auto Particle = SpawnObject<ParticleComponent>();
	Particle->SetSystem(ParticleSystem::MakeSystem<CloudParticle>());
	Particle->SetLocation(Vector(-116.f, -104.f, 44.f));
	
	auto Particle2 = SpawnObject<ParticleComponent>();
	Particle2->SetSystem(ParticleSystem::MakeSystem<CloudParticle>());
	Particle2->SetLocation(Vector(-60.f, -94.f, 44.f));

	auto smoke = SpawnObject<ParticleComponent>();
	smoke->SetSystem(ParticleSystem::MakeSystem<SmokeParticle>());
	smoke->SetLocation(Vector(71.f, -40.f, 11.f));

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
	static String object = R"~~~(
def execute() {
	var objId = CreateObject("VisibleObject", 1);
	SetModel(objId, "Cube");
	SetMaterial(objId, "hunter");
	SetLocation(objId, 5, 5, 2);
	#SetRotation(objId, 0, 0, 0);
	#SetScale(objId, 1, 1, 1);
	#DestroyObject(objId);
})~~~";
	if (!KeyDown) {
		InputMode = !InputMode;
		int id = ScriptCore::CompileScript(object.c_str());
		ScriptCore::EvaluateScript(id);
		ScriptCore::CleanScript(id);
	}
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
	Changing = KeyDown;
}

void TestPlayer::MouseMoved(float X, float Y)
{
	if (cursorState && !Changing) {
		Camera* cam = GetCamera();
		Rotator rot = cam->GetRotation();
		float y = rot.PitchDegrees();
		cam->SetRotation(Vector(
			rot.RollDegrees(),
			y + Y * mouseSens < 89.f && y + Y * mouseSens > -89.f ? y + Y * mouseSens : y,
			rot.YawDegrees() + X * mouseSens
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
	//Console::Log((BoxCol2->GetWorldLocation()).ToString() + " BoxCol2");
	//Console::Log((BoxCol->GetWorldLocation()).ToString() + " BoxCol2");
	//Console::Log((Box->GetWorldLocation()).ToString() + " Box");
	//Console::Log((GetWorldLocation()).ToString() + " Player");
	//Console::Log((PlayerCol->GetWorldLocation()).ToString() + " PlayerCol");

	Vector loc = Box->GetLocation();
	GetCamera()->SetLocation(Location + Vector(0.f, 0.f, 1.5f));
	Sky->SetLocation(Location);
	
	/*Vector listenerPos = Location;
	Vector listenerOrientation = GetCamera()->GetForwardVector();
	AudioManager::SetListener(listenerPos, GetCamera()->GetForwardVector(), GetCamera()->GetUpVector());*/
}

void TestPlayer::BeginPlay()
{

	//Terrain* terrain = ObjectManager::GetByRecord<Terrain>(0xA0005554);

	
	//ECS
	SystemsManager* systemsManager = IECS::GetSystemsManager();
	//Audio Testing
	IComponentArrayQuerySystem<AudioComponent>* audioComponentArraySystem = static_cast<IComponentArrayQuerySystem<AudioComponent>*> (systemsManager->GetSystemByName("AudioControllerSystem"));
	AudioComponent* audio = audioComponentArraySystem->AddComponentToSystem();
	
	//AudioControllerSystem* audioControllerSystem = static_cast<AudioControllerSystem*>(systemsManager->GetSystemByName("AudioControllerSystem"));
	//audioComponentID = audio->GetID();

	Vector audioPos = Vector(20.0f, 20.0f, 1.5f);
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
		DirLight->Location = Vector(0.f, 0.f, 0.f);
		DirLight->LightType = LIGHT_DIRECTIONAL;
		DirLight->Size = 3.f;
		DirLight->Intensity = 2.f;
		DirLight->Color = Vector(1.f);
		DirLight->Rotation = Vector(0.5, -0.5, 1.0).Normalize();

		//for (int i = 0; i < 50; i++)
		//{
		//	//Console::Log("Light addded " + std::to_string(i));
		//	float x = (float)(rand() % 100);
		//	float y = (float)(rand() % 100);
		//	//float s = 1.f - rand() / (float)RAND_MAX * 0.7f;

		//	LightComponent* light = lightSystem->AddComponentToSystem();
		//	light->Location = Vector(x, y, 2.f);
		//	light->LightType = LIGHT_POINT;
		//	light->Size = 5.f;
		//	light->Intensity = rand() / (float)RAND_MAX * 20.f;
		//	light->Color = Vector(x, y, 2.5f);
		//}
	}
	Console::Log("Hello beautiful world");
}

void TestPlayer::OnDestroyed()
{
}
