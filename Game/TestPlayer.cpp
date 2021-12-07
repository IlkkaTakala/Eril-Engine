#include "Interface/IRender.h"
#include "TestPlayer.h"
#include "Objects/MovementComponent.h"
#include "Objects/Terrain.h"
#include "Timer.h"
#include "Objects/InstancedObject.h"
#include "Hunter.h"
#include "TestUI.h"
#include <Interface/WindowManager.h>
#include <Interface/AudioManager.h>

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
static bool cursorState = true;
void TestPlayer::UseCursor(bool keydown)
{
	if (!keydown) {
		WindowManager::SetShowCursor(0, cursorState);
		cursorState = !cursorState;
	}
}

TestPlayer::TestPlayer() : Player()
{
	//ECS Example
	//ecsExample = SpawnObject<ECSExample>();

	mouseSens = 0.5f;
	Speed = 5.f;
	InputMode = true;
	spawnCounter = 0;

	//GetCamera()->SetLocation(INI->GetValue("Player", "Start"));
	//GetCamera()->SetRotation(INI->GetValue("Player", "Direction"));

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
	Mesh->GetModel()->SetAABB(AABB(Vector(-0.5f), Vector(0.5f)));
	

	//Player Movement
	Movement = SpawnObject<MovementComponent>();
	Movement->SetTarget(dynamic_cast<Actor*>(this));
	Movement->SetGravity(true);

	//Skybox
	Sky = SpawnObject<VisibleObject>();
	Sky->SetModel(MI->LoadData(Sky, "SkySphere"));
	Sky->GetModel()->SetMaterial(0, RI->LoadMaterialByName("Assets/Materials/Sky"));
	Sky->SetScale(Sky->GetScale() * 2.0f);

	//Testing UI
	auto ui = SpawnObject<TestUI>();
	UI::AddToScreen(ui, this);
}


//Handle Inputs
void TestPlayer::RunInputQ(float delta, bool KeyDown)
{
	Vector dir(0.0,0.0,1.0);
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



void TimeFunction(float d) 
{
}

/*
uint audioID = 0;
bool first = true;
Vector audioPos(10.0f, 10.0f, 1.0f);

void AudioTimer(float d)
{ 
	if (first)
	{
		audioID  = AudioManager::LoadAudio("clicketi.wav");
		AudioManager::SetAudioRelativity(audioID, false);
		first = false;
	}
	
	
	AudioManager::SetAudioGain(audioID, 0.5f);

	AudioManager::PlayAudio(audioID, audioPos);
	Timer::CreateTimer(7.0f, AudioTimer);
}

uint audioID2 = 0;
bool first2 = true;

void AudioTimer2(float d)
{

	if (first2)
	{
		audioID2 = AudioManager::LoadAudio("whistle.wav");
		first2 = false;
	}
	AudioManager::PlayAudio(audioID2);
	AudioManager::SetAudioGain(audioID2, 0.1f);


	Timer::CreateTimer(15.0f, AudioTimer2);
}


*/

void TestPlayer::Tick(float deltaTime)
{
	

	GetCamera()->SetLocation(Location + Vector(0.f, 0.f, 1.5f));
	GetCamera()->SetRotation(Rotation);
	Sky->SetLocation(Location);

	//audioPos.X += 4.5f * deltaTime;

	//Mesh->SetLocation(audioPos);

	//AudioManager::SetAudioPosition(audioID, audioPos);
	
	Vector listenerPos = Location;
	Vector listenerOrientation = GetCamera()->GetForwardVector();
	//Console::Log(std::to_string(listenerPos.X) + "," + std::to_string(listenerPos.Y) + "," + std::to_string(listenerPos.Z) + ", Rot: " + std::to_string(GetCamera()->GetForwardVector().X) + "," + std::to_string(GetCamera()->GetForwardVector().Y) + "," + std::to_string(GetCamera()->GetForwardVector().Z));
	AudioManager::SetListener(listenerPos, -GetCamera()->GetForwardVector(), -GetCamera()->GetUpVector());
}

void TestPlayer::BeginPlay()
{
	
	//AudioTimer(0.0f);


	//AudioTimer2(0.0f);

	printf("Spawned object\n");
	Timer::CreateTimer(5.f, TimeFunction, false);

	

	uint64 l = 0xABCDEF0123456789;
	uint32 h = (uint32)l;
	printf("0x%lx\n", h);
	
	
	Terrain* terrain = ObjectManager::GetByRecord<Terrain>(0xA0005554);

	
	//ECS
	SystemsManager* systemsManager = IECS::GetSystemsManager();
	//Audio Testing
	IComponentArrayQuerySystem<AudioComponent>* audioComponentArraySystem = static_cast<IComponentArrayQuerySystem<AudioComponent>*> (systemsManager->GetSystemByName("AudioControllerSystem"));
	AudioControllerSystem* audioControllerSystem = static_cast<AudioControllerSystem*>(systemsManager->GetSystemByName("AudioControllerSystem"));

	AudioComponent* audio = audioComponentArraySystem->AddComponentToSystem();
	audioComponentID = audio->GetID();

	Vector audioPos = Vector(20.0f, 20.0f, terrain->GetHeight(20.0f, 20.0f) + 1.5f);
	audio->SetSourceID(audioControllerSystem->LoadAudioFile("clicketi.WAV"));
	audio->SetPosition(audioPos);
	Mesh->SetLocation(audioPos);
	audio->SetGain(1.0f);
	audio->SetPitch(1.0f);
	audio->SetLooping(true);
	audio->SetSourceRelative(true);


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

		for (int i = 0; i < 10; i++)
		{
			//Console::Log("Light addded " + std::to_string(i));
			float x = (float)(rand() % 100);
			float y = (float)(rand() % 100);
			y = 1;
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
