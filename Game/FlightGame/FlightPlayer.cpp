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
#include "Objects/ColliderComponent.h"
#include "Objects/ParticleComponent.h"
#include "TestArea/CloudParticle.h"

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
	Speed = 10.f;
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
	Movement->SetFlightMaxSpeed(Speed);
	Movement->SetAirBrake(2000.f);
	Movement->SetAcceleration(500.f);
	Movement->SetAirControl(1.f);

	auto pc = SpawnObject<ColliderComponent>();
	pc->SetType(2);
	pc->SetTarget(this);
	pc->SetMovementTarget(Movement);
	pc->SetSize(AABB({-0.5f, -0.5f, 0.f}, { 0.5f, 0.5f, 1.f }));
	pc->SetLocation({ 0.f, 0.f, 0.5f });
	SetRotation(0.f);

	//GetCamera()->SetPostProcess("PostProcessForest");
	int AreaSize = 700;

	for (int i = 0; i < 500; i++) {
		auto m = SpawnObject<VisibleObject>();
		m->SetModel("Island");
		m->GetModel()->SetMaterial(0, RI->LoadMaterialByName("Assets/Materials/Island"));
		m->GetModel()->SetMaterial(1, RI->LoadMaterialByName("Assets/Materials/grass"));

		float rad = 1.0f;
		float s = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.0f * PI;
		float t = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.0f * PI;


		float x = RandomFloatInRange(-AreaSize, AreaSize);
		float y = RandomFloatInRange(-AreaSize, AreaSize);
		float z = RandomFloatInRange(-AreaSize, AreaSize);
		Vector loc(x, y, z * 0.3f);

		auto c = SpawnObject<ColliderComponent>();
		c->SetType(0);
		c->SetTarget(c);
		c->SetSize(m->GetModel()->GetAABB());
		c->SetLocation({ 0, 0, 0 });
		m->AddComponent(c);
		m->SetLocation(loc);
		m->SetRotation({ 0.f, 0.f, static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 360.f });
	}

	for (int i = 0; i < 100; i++) {

		float rad = 1.0f;
		float s = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.0f * PI;
		float t = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2.0f * PI;


		float x = RandomFloatInRange(-AreaSize, AreaSize);
		float y = RandomFloatInRange(-AreaSize, AreaSize);
		float z = RandomFloatInRange(-AreaSize, AreaSize);
		Vector loc(x, y, z * 0.3f);

		auto psys = SpawnObject<ParticleComponent>();
		psys->SetSystem(ParticleSystem::MakeSystem<CloudParticle>());
		psys->SetLocation(loc);
	}
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
	Movement->AddInput(-GetCamera()->GetForwardVector());
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
	Movement->AddInput(GetCamera()->GetForwardVector());
}

void FlightPlayer::RunInputSpace(bool KeyDown)
{
	/*if (!Movement->IsInAir() && KeyDown) {
		Movement->AddImpulse(Vector(0.f, 0.f, 200.f));
	}*/
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
		Movement->SetMaxSpeed(20.f);
		Movement->SetFlightMaxSpeed(20.f);
	}
	else {
		Movement->SetMaxSpeed(Speed);
		Movement->SetFlightMaxSpeed(Speed);
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
	if (cursorState) SetRotation(Vector(
		rot.X, 
		rot.Y + Y * mouseSens < 89.f && rot.Y + Y * mouseSens > -89.f ? rot.Y + Y * mouseSens : rot.Y,
		rot.Z + X * mouseSens));
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

		std::vector<LightComponent>* dirLight = IECS::GetComponentManager()->GetComponentVector<LightComponent>(IECS::GetComponentManager()->GetTypeIdByName("LightComponent"));

		if (dirLight->size() > 0) {
			LightComponent& l = dirLight->at(0);
			l.Rotation = Vector::RotateByAxis(l.Rotation, {1, 0, 0}, delta * 0.1);

			Sky->GetModel()->GetMaterial(0)->SetParameter("sunDirection", -l.Rotation);
		}
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


	//Lights Testing
	SystemsManager* WorldSystemsManager = IECS::GetSystemsManager();
	ComponentManager* WorldComponentManager = IECS::GetComponentManager();
	EntityManager* WorldEntityManager = IECS::GetEntityManager();


	IComponentArrayQuerySystem<LightComponent>* lightSystem = static_cast<IComponentArrayQuerySystem<LightComponent>*> (WorldSystemsManager->GetSystemByName("LightControllerSystem"));

	if (lightSystem != nullptr)
	{
		LightComponent* light = lightSystem->AddComponentToSystem();
		light->Location = Vector();
		light->LightType = LIGHT_DIRECTIONAL;
		light->Size = 5.f;
		light->Intensity = 6.0f;
		light->Color = Vector(1.f);
		light->Rotation = Vector(0, 0, -1);
	}
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






