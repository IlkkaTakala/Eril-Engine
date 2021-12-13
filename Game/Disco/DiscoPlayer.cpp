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
#include <ECS/Components/AudioComponent.h>
#include <ECS/Systems/AudioControllerSystem.h>
#include <Disco/DiscoLightComponent.h>
#include <Disco/DiscoLightSystem.h>


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


	//Audio Source Model
	AudioSourceMesh = SpawnObject<VisibleObject>();
	AudioSourceMesh->SetModel("sphere");
	AudioSourceMesh->GetModel()->SetAABB(AABB(Vector(-0.25f), Vector(0.25f)));
	AudioSourceMesh->SetScale(0.5f);

	//Audio Source Model
	StaticAudioSource = SpawnObject<VisibleObject>();
	StaticAudioSource->SetModel("Cube");
	StaticAudioSource->GetModel()->SetAABB(AABB(Vector(-0.5f), Vector(0.5f)));
	StaticAudioSource->SetScale(1.0f);
	StaticAudioSource->SetLocation(Vector(0.0f, -35.0f, 3.6f));
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


void DiscoPlayer::ChangeAudioSourceAngle(float d)
{
	AudioDir.X = 0.0f - (static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
	AudioDir.Y = 0.0f - (static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
	if (AudioDir.X < 0.2f && AudioDir.X >= 0.0f)
	{
		AudioDir.X = 0.2f;
	}
	if (AudioDir.X <= 0.0f && AudioDir.X > -0.2f)
	{
		AudioDir.X = -0.2f;
	}
	if (AudioDir.Y < 0.2f && AudioDir.Y >= 0.0f)
	{
		AudioDir.Y = 0.2f;
	}
	if (AudioDir.Y <= 0.0f && AudioDir.Y > -0.2f)
	{
		AudioDir.Y = -0.2f;
	}

}

void DiscoPlayer::ClampLocation()
{
	//Room Dimensions
	//X: -24, 24
	//Y: -45, 45
	//Z: 0, 32

	if (Location.X < -23)
	{
		Location.X = -23;
		Vector dir(1.0, 0.0, 0.0);
		Movement->AddInput(dir.Normalize());
	}
	if (Location.X > 23)
	{
		Location.X = 23;
		Vector dir(-1.0, 0.0, 0.0);
		Movement->AddInput(dir.Normalize());
	}
	if (Location.Y < -44)
	{
		Location.Y = -44;
		Vector dir(0.0, 1.0, 0.0);
		Movement->AddInput(dir.Normalize());
	}
	if (Location.Y > 44)
	{
		Location.Y = 44;
		Vector dir(0.0, -1.0, 0.0);
		Movement->AddInput(dir.Normalize());
	}
	if (Location.Z < 1)
	{
		Location.Z = 1;
		Vector dir(0.0, 0.0, 1.0);
		Movement->AddInput(dir.Normalize());
	}
	if (Location.Z > 31)
	{
		Location.Z = 31;
		Vector dir(0.0, 0.0, -1.0);
		Movement->AddInput(dir.Normalize());
	}
}


void DiscoPlayer::Tick(float deltaTime)
{
	ClampLocation();

	GetCamera()->SetLocation(Location + Vector(0.f, 0.f, 1.5f));
	GetCamera()->SetRotation(Rotation);



	AudioComponent &audioComponent = IECS::GetComponentManager()->GetComponentVector<AudioComponent>(IECS::GetComponentManager()->GetTypeIdByName("AudioComponent"))->at(AudioID);
	AudioS += deltaTime * AudioDir.X * 0.75f;
	AudioT += deltaTime * AudioDir.Y * 0.75f;
	float x = cos(AudioS) * sin(AudioT);
	float y = sin(AudioS) * sin(AudioT);
	float z = cos(AudioT);
	Vector loc(x, y, z);
	loc *= Vector(24, 45, 16);
	loc.Z += 16;
	Vector previousLoc = audioComponent.GetPosition();
	Vector velocity = (loc - previousLoc) / deltaTime;
	audioComponent.SetPosition(loc);
	audioComponent.SetVelocity(velocity);
	AudioSourceMesh->SetLocation(loc);

	Vector listenerPos = Location;
	Vector listenerOrientation = GetCamera()->GetForwardVector();
	AudioManager::SetListener(listenerPos, -GetCamera()->GetForwardVector(), -GetCamera()->GetUpVector());
	

	
}

void DiscoPlayer::BeginPlay()
{
	//Lights Testing
	SystemsManager* WorldSystemsManager = IECS::GetSystemsManager();
	ComponentManager* WorldComponentManager = IECS::GetComponentManager();
	EntityManager* WorldEntityManager = IECS::GetEntityManager();

	WorldComponentManager->CreateComponentTypeStorage<DiscoLightComponent>("DiscoLightComponent");

	DiscoLightSystem* DiscoLightControl = new DiscoLightSystem(WorldEntityManager, WorldComponentManager, "DiscoLightComponent");
	int systemIndex = WorldSystemsManager->AddSystem(DiscoLightControl, "DiscoLightSystem");
	WorldSystemsManager->AddWantedComponentType(systemIndex, "DiscoLightComponent");




	IComponentArrayQuerySystem<LightComponent>* lightSystem = static_cast<IComponentArrayQuerySystem<LightComponent>*> (WorldSystemsManager->GetSystemByName("LightControllerSystem"));

	if (lightSystem != nullptr)
	{
		for (int i = 0; i < 100; i++)
		{

			//Circum
			//2*PI*radius

			float rad = 1.0f;
			float s = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.0f * PI;
			float t = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.0f * PI;


			float x = rad * cos(s) * sin(t);
			float y = rad * sin(s) * sin(t);
			float z = rad * cos(t);
			
			Vector loc(x, y, z);
			loc *= Vector(24, 45, 16);
			loc.Z += 16;

			//Room Dimensions
			//X: -24, 24
			//Y: -45, 45
			//Z: 0, 32
			
			//Console::Log("Light addded " + std::to_string(i));
			//int x = rand() % 44;
			//int y = rand() % 86;
			//int z = rand() % 30;

			float r = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)); // rand between 0-1
			float g = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)); // rand between 0-1
			float b = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)); // rand between 0-1

			//float s = 1.f - rand() / (float)RAND_MAX * 0.7f;

			LightComponent* light = lightSystem->AddComponentToSystem();
			light->Location = loc;
			light->LightType = LIGHT_POINT;
			light->Size = 10.f;
			light->Intensity = rand() / (float)RAND_MAX * 20.f + 10.0f;
			light->Color = Vector(r, g, b);
			//lightSystem->GetComponentVector()->at(i).Color = 

			lightposition.emplace(light->GetID(), std::tuple<float, float>(s, t));
			

			DiscoLightComponent* Discolight = DiscoLightControl->AddComponentToSystem();

			Discolight->LightID = light->GetID();
			Discolight->s = s;
			Discolight->t = t;
		}
	}

	//Room Dimensions
	//X: -24, 24
	//Y: -45, 45
	//Z: 0, 32

	IComponentArrayQuerySystem<AudioComponent>* audiosystem = static_cast<IComponentArrayQuerySystem<AudioComponent>*> (WorldSystemsManager->GetSystemByName("AudioControllerSystem"));


	AudioComponent* audioComponent = audiosystem->AddComponentToSystem();
	Vector audioPos = Vector(-20.0f, -41.0f,  15.0f);
	audioComponent->SetSourceID(AudioManager::LoadAudio("siren.WAV"));
	audioComponent->SetPosition(audioPos);
	audioComponent->SetGain(1.0f);
	audioComponent->SetPitch(1.0f);
	audioComponent->SetLooping(true);
	audioComponent->SetSourceRelative(false);
	audioComponent->Play();

	AudioID = audioComponent->GetID();
	AudioS = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.0f * PI;
	AudioT = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.0f * PI;
	AudioDir.X = 1.0f;
	AudioDir.Y = 1.0f;
	AudioDir.Z = 1.0f;

	Timer::CreateTimer<DiscoPlayer>(7.5f, &DiscoPlayer::ChangeAudioSourceAngle, this, true, false);


	audioComponent = audiosystem->AddComponentToSystem();
	audioPos = Vector(0.0f, -35.0f, 4.0f);
	audioComponent->SetSourceID(AudioManager::LoadAudio("clicketi.WAV"));
	audioComponent->SetPosition(audioPos);
	audioComponent->SetGain(1.0f);
	audioComponent->SetPitch(1.5f);
	audioComponent->SetLooping(true);
	audioComponent->SetSourceRelative(false);
	audioComponent->Play();

	Console::Log("Hello colorful world");
}

void DiscoPlayer::OnDestroyed()
{
}
