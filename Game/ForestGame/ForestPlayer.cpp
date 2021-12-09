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

//ECS
#include <Interface/IECS.h>
#include <ECS_Examples/ECSExample.h>
#include <ECS/Components/LightComponent.h>
#include <ECS/Systems/LightControllerSystem.h>


void ForestPlayer::OpenConsole(bool) {
	Console::Create();
}

void ForestPlayer::UseCursor(bool keydown)
{
	if (keydown && pause == nullptr) {
		WindowManager::SetShowCursor(0, cursorState);
		cursorState = !cursorState;
	}
}

ForestPlayer::ForestPlayer() : Player()
{
	mouseSens = 0.2f;
	Speed = 15.f;

	InputMode = true;
	cursorState = true;
	pause = nullptr;
	spawnCounter = 0;

	//GetCamera()->SetLocation(INI->GetValue("Player", "Start"));
	//GetCamera()->SetRotation(INI->GetValue("Player", "Direction"));

	//Reqister used Inputs
	II->RegisterKeyContinuousInput(87, &ForestPlayer::RunInputW, this);
	II->RegisterKeyContinuousInput(65, &ForestPlayer::RunInputA, this);
	II->RegisterKeyContinuousInput(83, &ForestPlayer::RunInputS, this);
	II->RegisterKeyContinuousInput(68, &ForestPlayer::RunInputD, this);
	II->RegisterKeyInput(32, &ForestPlayer::RunInputSpace, this);
	II->RegisterKeyInput(340, &ForestPlayer::RunInputShift, this);
	II->RegisterKeyInput(0, &ForestPlayer::LeftMouseDown, this);
	II->RegisterKeyInput(49, &ForestPlayer::InputOne, this);
	II->RegisterKeyInput(50, &ForestPlayer::InputTwo, this);
	II->RegisterKeyInput(256, &ForestPlayer::InputExit, this);
	II->RegisterKeyInput(257, &ForestPlayer::OpenConsole, this);
	II->RegisterMouseInput(0, &ForestPlayer::MouseMoved, this);
	II->RegisterKeyInput(69, &ForestPlayer::ItemPickE, this);
	II->RegisterKeyInput(81, &ForestPlayer::ItemThrowQ, this);


	//Player Model
	Mesh = SpawnObject<VisibleObject>();
	Mesh->SetModel("Cube");
	Mesh->GetModel()->SetAABB(AABB(Vector(-0.5f), Vector(0.5f)));

	Movement = SpawnObject<MovementComponent>();
	Movement->SetTarget(dynamic_cast<Actor*>(this));
	Movement->SetGravity(true);
	Movement->SetGround(ObjectManager::GetByRecord<Terrain>(0xA0001111));

	//Skybox
	Sky = SpawnObject<VisibleObject>();
	Sky->SetModel(MI->LoadData(Sky, "SkySphere"));
	Sky->GetModel()->SetMaterial(0, RI->LoadMaterialByName("Assets/Materials/Sky"));

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
	if (KeyDown)
		InputMode = !InputMode;
}

void ForestPlayer::InputTwo(bool KeyDown)
{
	if (KeyDown)
		InputMode = !InputMode;
	Scene::OpenLevel("Assets/Maps/test");
}

void ForestPlayer::RunInputShift(bool KeyDown)
{
	if (KeyDown) Movement->SetMaxSpeed(10.f);
	else Movement->SetMaxSpeed(5.f);
}


void ForestPlayer::LeftMouseDown(bool)
{
}

void ForestPlayer::RightMouseDown(bool KeyDown)
{
	
}

void ForestPlayer::ItemThrowQ(bool KeyDown)
{
	if (!KeyDown)
	{
		if (Items.size() > 0) {
			auto it = SpawnObject<PlaceableItem>();
			it->SetLocation(GetLocation() + Vector(0.f, 0.f, 1.f));
			it->SetScale(0.2f);
			it->Move->SetPhysics(true);
			it->Move->SetMaxSpeed(10.0f);
			it->Move->AddImpulse(-GetCamera()->GetForwardVector() * 10.f);
			Items[0]->DestroyObject();
			Items.erase(Items.begin());
		}
		Console::Log("Items: " + std::to_string(Items.size()));
	}
}

void ForestPlayer::ItemPickE(bool KeyDown)
{
	if (!KeyDown) {
		Items.push_back(SpawnObject<Item>());
		Console::Log("Items: " + std::to_string(Items.size()));
	}
}


void ForestPlayer::MouseMoved(float X, float Y)
{
	const Vector& rot = Rotation;
	if (cursorState) SetRotation(Vector(rot.X + X * mouseSens, rot.Y + Y * mouseSens < 89.f && rot.Y + Y * mouseSens > -89.f ? rot.Y + Y * mouseSens : rot.Y, rot.Z));
}

void ForestPlayer::InputExit(bool down)
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

void ForestPlayer::Tick(float)
{
	GetCamera()->SetLocation(Location + Vector(0.f, 0.f, 1.5f));
	GetCamera()->SetRotation(Rotation);
	Sky->SetLocation(Location);
}

void ForestPlayer::BeginPlay()
{
	RecordInt r = GetRecord();//0xABCDEF0123456789;

	if (r.GetSpawnState() == Constants::Record::SPAWNED) {
		Console::Log("Spawned object");
	}
	uint64 l = 0xABCDEF0123456789;
	uint32 h = (uint32)l;
	Console::Log("0x%lx " + std::to_string(h));

	Console::Log("Hello beautiful world");
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

PlaceableItem::PlaceableItem() : Actor()
{

}

void PlaceableItem::BeginPlay()
{
	Move = SpawnObject<MovementComponent>();
	Move->SetTarget(this);
	Move->SetPhysics(true);
	Move->SetBrake(1.1f);
	Mesh = SpawnObject<VisibleObject>();
	AddComponent(Mesh);
	Mesh->SetModel("candyCane");
	Mesh->GetModel()->SetMaterial(0, RI->LoadMaterialByName("Assets/Materials/candy"));
	/*
	printf("0x%lx\n", h);


	Terrain* terrain = ObjectManager::GetByRecord<Terrain>(0xA0005554);

	//Lights Testing
	SystemsManager* systemsManager = IECS::GetSystemsManager();
	IComponentArrayQuerySystem<LightComponent>* lightSystem = static_cast<IComponentArrayQuerySystem<LightComponent>*> (systemsManager->GetSystemByName("LightControllerSystem"));

	if (lightSystem != nullptr)
	{
		LightComponent* DirLight = lightSystem->AddComponentToSystem("LightComponent");
		DirLight->Location = Vector(0.f, 0.f, 1.f);
		DirLight->LightType = LIGHT_DIRECTIONAL;
		DirLight->Size = 3.f;
		DirLight->Intensity = 1.f;
		DirLight->Color = Vector(1.f);
		DirLight->Rotation = Vector(0.5, 0.5, -0.5);

		for (int i = 0; i < 10; i++)
		{
			//Console::Log("Light addded " + std::to_string(i));
			float x = rand() % 100;
			float y = rand() % 100;
			//float s = 1.f - rand() / (float)RAND_MAX * 0.7f;

			LightComponent* light = lightSystem->AddComponentToSystem("LightComponent");
			light->Location = Vector(x, y, terrain->GetHeight(x, y));
			light->LightType = LIGHT_POINT;
			light->Size = 5.f;
			light->Intensity = rand() / (float)RAND_MAX * 20.f;
			light->Color = Vector(x, y, 2.5f);
		}
	}*/
	Console::Log("Hello beautiful world");
}

void ForestPlayer::OnDestroyed()
{
}

void PlaceableItem::DestroyObject()
{
	
}
