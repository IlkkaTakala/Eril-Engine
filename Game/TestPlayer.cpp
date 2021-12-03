#include "Interface/IRender.h"
#include "TestPlayer.h"
#include "Objects/MovementComponent.h"
#include "Objects/Terrain.h"
#include "Timer.h"
#include "Objects/InstancedObject.h"
#include "Hunter.h"
#include "Objects/Actor.h"
#include "TestUI.h"
#include <Interface/WindowManager.h>

//ECS
#include <Interface/IECS.h>
#include <ECS_Examples/ECSExample.h>
#include <ECS/Components/LightComponent.h>
#include <ECS/Systems/LightControllerSystem.h>


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
	mouseSens = 0.2f;
	Speed = 15.f;

	InputMode = true;
	spawnCounter = 0;

	//GetCamera()->SetLocation(INI->GetValue("Player", "Start"));
	//GetCamera()->SetRotation(INI->GetValue("Player", "Direction"));

	//Reqister used Inputs
	//II->RegisterKeyContinuousInput(81, &TestPlayer::RunInputQ, this);
	//II->RegisterKeyContinuousInput(90, &TestPlayer::RunInputZ, this);
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
	II->RegisterKeyInput(69, &TestPlayer::ItemPickE, this);
	II->RegisterKeyInput(81, &TestPlayer::ItemThrowQ, this);


	//Player Model
	/*Mesh = SpawnObject<VisibleObject>();
	Mesh->SetModel("Cube");
	Mesh->GetModel()->SetAABB(AABB(Vector(-0.5f), Vector(0.5f)));

	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			terra[y * 2 + x] = SpawnObject<Terrain>();
			terra[y * 2 + x]->InitTerrain(100, Vector(100.f, 100.f, 1.f), Vector(-100.f * x, -100.f * y, 0.f));
		}
	}*/


	Movement = SpawnObject<MovementComponent>();
	Movement->SetTarget(dynamic_cast<Actor*>(this));
	Movement->SetGravity(true);
	Movement->SetGround(ObjectManager::GetByRecord<Terrain>(0xA0001111));

	//Skybox
	Sky = SpawnObject<VisibleObject>();
	Sky->SetModel(MI->LoadData(Sky, "SkySphere"));
	Sky->GetModel()->SetMaterial(0, RI->LoadMaterialByName("Assets/Materials/Sky"));

	/* {
		Trees = SpawnObject<InstancedObject>();
		Trees->SetModel(MI->LoadData(Trees, "tree"));
		Trees->GetModel()->SetMaterial(0, RI->LoadMaterialByName("Assets/Materials/tree"));
		Trees->GetModel()->SetMaterial(1, RI->LoadMaterialByName("Assets/Materials/leaves"));
		Trees->GetModel()->SetAABB(AABB(Vector(-100.f), Vector(100.f)));

		int count = 100;
		Transformation* arr = new Transformation[count]();
		for (int i = 0; i < count; i++)
		{
			float x = rand() % 100 - 50.f;
			float y = rand() % 100 - 50.f;
			float s = 1.f - rand() / (float)RAND_MAX * 0.7f;
			arr[i].Location = Vector(x, y, terra[0]->GetHeight(x, y) - 0.2f);
			arr[i].Scale = Vector(s);
		}
		Trees->AddInstances(count, arr);
		delete[] arr;
		
		Rocks = SpawnObject<InstancedObject>();
		Rocks->SetModel(MI->LoadData(Rocks, "rock"));
		Rocks->GetModel()->SetMaterial(0, RI->LoadMaterialByName("Assets/Materials/rock"));
		Rocks->GetModel()->SetAABB(AABB(Vector(-100.f), Vector(100.f)));

		count = 20;
		arr = new Transformation[count]();
		for (int i = 0; i < count; i++)
		{
			float x = rand() % 100 - 50.f;
			float y = rand() % 100 - 50.f;
			arr[i].Location = Vector(x, y, terra[0]->GetHeight(x, y) + 0.6f);
			arr[i].Scale = Vector(1.0f);
		}
		Rocks->AddInstances(count, arr);
		delete[] arr;
		
		Shacks = SpawnObject<InstancedObject>();
		Shacks->SetModel(MI->LoadData(Shacks, "shack"));
		Shacks->GetModel()->SetMaterial(0, RI->LoadMaterialByName("Assets/Materials/shack"));
		Shacks->GetModel()->SetMaterial(1, RI->LoadMaterialByName("Assets/Materials/shack"));
		Shacks->GetModel()->SetAABB(AABB(Vector(-100.f), Vector(100.f)));

		count = 3;
		arr = new Transformation[count]();
		for (int i = 0; i < count; i++)
		{
			float x = rand() % 100 - 50.f;
			float y = rand() % 100 - 50.f;
			arr[i].Location = Vector(x, y, terra[0]->GetHeight(x, y) + 0.1f);
			arr[i].Scale = Vector(1.0f);
		}
		Shacks->AddInstances(count, arr);
		delete[] arr;

		Grass = SpawnObject<InstancedObject>();
		Grass->SetModel(MI->LoadData(Grass, "grass"));
		Grass->GetModel()->SetMaterial(0, RI->LoadMaterialByName("Assets/Materials/grass"));
		Grass->GetModel()->SetAABB(AABB(Vector(-100.f), Vector(100.f)));

		count = 1000;
		arr = new Transformation[count]();

		for (int i = 0; i < count; i++)
		{
			float x = rand() % 100 - 50.f;
			float y = rand() % 100 - 50.f;
			Vector normal = terra[0]->GetNormal(x, y);
			arr[i].Location = Vector(x, y, terra[0]->GetHeight(x, y)) - normal * 0.1;
			arr[i].Scale = Vector(0.6f);
			Vector up(0.f, 0.f, 1.f);
			Vector axis = Vector::Cross(normal, up).Normalize();
			float angle = acos(Vector::Dot(up, normal));
			arr[i].Rotation = Vector::toEuler(axis, angle) * 180.f / PI;
		}
		Grass->AddInstances(count, arr);
		delete[] arr;


		Candy = SpawnObject<InstancedObject>();
		Candy->SetModel(MI->LoadData(Candy, "candyCane"));
		Candy->GetModel()->SetMaterial(0, RI->LoadMaterialByName("Assets/Materials/candy"));
		Candy->GetModel()->SetAABB(AABB(Vector(-100.f), Vector(100.f)));

		count = 50;
		arr = new Transformation[count]();
		for (int i = 0; i < count; i++)
		{
			float x = rand() % 100 - 50.f;
			float y = rand() % 100 - 50.f;
			float s = (1.f - rand() / (float)RAND_MAX * 0.4f);
			Vector normal = terra[0]->GetNormal(x, y);
			arr[i].Location = Vector(x, y, terra[0]->GetHeight(x, y) + 0.8f);
			arr[i].Scale = Vector(0.2f);
			Vector up(0.f, 1.f, 0.f);
			Vector axis = Vector::Cross(normal, up).Normalize();
			float angle = acos(Vector::Dot(up, normal));
			arr[i].Rotation = Vector::toEuler(axis, angle) * 180.f / PI;
		}
		Candy->AddInstances(count, arr);
		delete[] arr;
	}

	hunt = SpawnObject<Hunter>();
	hunt->SetLocation(Vector(10.f, 0.f, 0.f));
	hunt->move->SetGround(terra[0]);*/
	

}


//Handle Inputs
//void TestPlayer::RunInputQ(float delta, bool KeyDown)
//{
//	Vector dir(0.0,0.0,1.0);
//	Movement->AddInput(dir.Normalize());
//}
//void TestPlayer::RunInputZ(float delta, bool KeyDown)
//{
//	Vector dir(0.0, 0.0, -1.0);
//	Movement->AddInput(dir.Normalize());
//}

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
	if (!Movement->IsInAir() && KeyDown) {
		Movement->AddImpulse(Vector(0.f, 0.f, 200.f));
	}
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

void TestPlayer::ItemThrowQ(bool KeyDown)
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

void TestPlayer::ItemPickE(bool KeyDown)
{
	if (!KeyDown) {
		Items.push_back(SpawnObject<Item>());
		Console::Log("Items: " + std::to_string(Items.size()));
	}
}


void TestPlayer::MouseMoved(float X, float Y)
{
	const Vector& rot = Rotation;
	if (cursorState) SetRotation(Vector(rot.X + X * mouseSens, rot.Y + Y * mouseSens < 89.f && rot.Y + Y * mouseSens > -89.f ? rot.Y + Y * mouseSens : rot.Y, rot.Z));
}

void TestPlayer::Tick(float)
{
	GetCamera()->SetLocation(Location + Vector(0.f, 0.f, 1.5f));
	GetCamera()->SetRotation(Rotation);
	Sky->SetLocation(Location);
}

void TimeFunction(float d) {
	Console::Log("Hello, timer ran");
}




void TestPlayer::BeginPlay()
{
	Timer::CreateTimer(2.f, &TimeFunction, true);
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
	Mesh->GetModel()->SetMaterial(0, RI->LoadMaterialByName("Assets/Materials/rock"));
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

void PlaceableItem::DestroyObject()
{
	
}
