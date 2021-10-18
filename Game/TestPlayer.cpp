#include "IRender.h"
#include "LightData.h"
#include "TestPlayer.h"
#include "Objects/MovementComponent.h"
#include "FallingCube.h"
#include "Objects/Terrain.h"
#include "Timer.h"
#include "Objects/InstancedObject.h"

TestPlayer::TestPlayer() : Player()
{
	mouseSens = 0.5f;
	Speed = 5.f;
	LightMode = false;
	spawnCounter = 0;
	//GetCamera()->SetLocation(INI->GetValue("Player", "Start"));
	//GetCamera()->SetRotation(INI->GetValue("Player", "Direction"));

	II->RegisterKeyInput(87, &TestPlayer::RunInputW, this);
	II->RegisterKeyInput(65, &TestPlayer::RunInputA, this);
	II->RegisterKeyInput(83, &TestPlayer::RunInputS, this);
	II->RegisterKeyInput(68, &TestPlayer::RunInputD, this);
	II->RegisterKeyInput(32, &TestPlayer::RunInputSpace, this);
	II->RegisterKeyInput(340, &TestPlayer::RunInputShift, this);
	II->RegisterKeyInput(0x01, &TestPlayer::LeftMouseDown, this);
	II->RegisterKeyInput(49, &TestPlayer::InputOne, this);
	II->RegisterKeyInput(50, &TestPlayer::InputTwo, this);
	II->RegisterKeyInput(256, &TestPlayer::InputExit, this);
	II->RegisterMouseInput(0, &TestPlayer::MouseMoved, this);

	SetModel("Cube");
	RenderData->SetAABB(AABB(Vector(-0.5f), Vector(0.5f)));

	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			terra[y * 2 + x] = SpawnObject<Terrain>();
			terra[y * 2 + x]->InitTerrain(1000, Vector(100.f, 100.f, 1.f), Vector(-100.f * x, -100.f * y, 0.f));
		}
	}
	Movement = SpawnObject<MovementComponent>();
	Movement->SetTarget(dynamic_cast<Actor*>(this));
	Movement->SetGravity(true);
	Movement->SetGround(terra[0]);

	Sky = SpawnObject<VisibleObject>();
	Sky->SetModel(MI->LoadData(Sky, "SkySphere"));
	Sky->GetModel()->SetMaterial(0, RI->LoadMaterialByName("Shaders/Materials/Sky"));

	Trees = SpawnObject<InstancedObject>();
	Trees->SetModel(MI->LoadData(Trees, "tree"));
	Trees->GetModel()->SetMaterial(0, RI->LoadMaterialByName("Shaders/Materials/tree"));
	Trees->GetModel()->SetMaterial(1, RI->LoadMaterialByName("Shaders/Materials/leaves"));

	int count = 100;
	Transformation* arr = new Transformation[count]();
	for (int i = 0; i < count; i++)
	{
		float x = rand() % 200 - 100.f;
		float y = rand() % 200 - 100.f;
		arr[i].Location = Vector(x, y, terra[0]->GetHeight(x, y));
		arr[i].Scale = Vector(1.f, 1.f, 1.f);
	}

	Trees->AddInstances(count, arr);
	delete[] arr; 
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

void TestPlayer::RunInputSpace(float delta, bool KeyDown)
{
	if (!Movement->IsInAir() && KeyDown)
		Movement->AddImpulse(Vector(0.f, 0.f, 300.f));
}

void TestPlayer::InputOne(float delta, bool KeyDown)
{
	if (LightMode == true) return;

	DirLight->Enable();

	for (auto const& l : Lights) {
		l->Disable();
	}
	LightMode = true;
}

void TestPlayer::InputTwo(float delta, bool KeyDown)
{
	if (LightMode == false) return;

	DirLight->Disable();

	for (auto const& l : Lights) {
		l->Enable();
	}
	LightMode = false;
}

void TestPlayer::RunInputShift(float delta, bool KeyDown)
{
	if (KeyDown) Movement->SetMaxSpeed(20.f);
	else Movement->SetMaxSpeed(10.f);
}


void TestPlayer::LeftMouseDown(float delta, bool)
{

}

void TestPlayer::RightMouseDown(float delta, bool KeyDown)
{

}

void TestPlayer::MouseMoved(float X, float Y)
{
	const Vector& rot = Rotation;
	SetRotation(Vector(rot.X + X * mouseSens, rot.Y + Y * mouseSens < 89.f && rot.Y + Y * mouseSens > -89.f ? rot.Y + Y * mouseSens : rot.Y, rot.Z));
}

void TestPlayer::Tick(float)
{
	GetCamera()->SetLocation(Location + Vector(0.f, 0.f, 1.5f));
	GetCamera()->SetRotation(Rotation);
	Sky->SetLocation(Location);

	/*if (spawnCounter++ == 40) {

		FallingCube* obj = SpawnObject<FallingCube>();

		float size = 80.f;
		float rx = (float)rand() / (float)RAND_MAX - 0.5f;
		float ry = (float)rand() / (float)RAND_MAX - 0.5f;
		float rz = (float)rand() / (float)RAND_MAX;
		obj->SetLocation(Vector(rx * size, ry * size, 15.f));

		spawnCounter = 0;
	}*/
}

#include "Objects/Actor.h"
#include "Objects/VisibleObject.h"

void TimeFunction(float d) {
	printf("Hello, timer ran\n");
}

void TestPlayer::BeginPlay()
{
	LightMode = true;

	DirLight = SpawnObject<Light>();
	DirLight->Data.Location = Vector(0.f);
	DirLight->Data.Type = LIGHT_DIRECTIONAL;
	DirLight->Data.Size = 7.0;
	DirLight->Data.Intensity = 5.0;
	DirLight->Data.Rotation = Vector(0.0, 45.0, -45.0);

	LastSphere = SpawnObject<Actor>();
	LastSphere->SetModel("Cube");
	LastSphere->SetLocation(Vector(3.5f, 0.0f, 0.0f));
	LastSphere->SetScale(Vector(1.f, 1.f, 1.f));
	LastSphere->GetModel()->SetMaterial(0, RI->LoadMaterialByName("Shaders/rocks"));

	/*LastSphere2 = SpawnObject<Actor>();
	LastSphere2->SetModel("sphere");
	LastSphere2->SetLocation(Vector(3.f, 0.0f, 0.0f));
	LastSphere2->GetModel()->SetMaterial(0, RI->LoadMaterialByName("Shaders/metal"));*/

	Timer::CreateTimer(5.f, TimeFunction, false);

	/*int count = 2000;
	Transformation* arr = new Transformation[count]();
	for (int i = 0; i < count; i++)
	{
		arr[i].Location = Vector(rand() % 200 - 100.f, rand() % 200 - 100.f, rand() % 200 - 100.f);
		arr[i].Scale = Vector(1.f, 1.f, 1.f);
	}

	Reflecting->AddInstances(count, arr);
	delete[] arr;*/

	/*Reflecting = SpawnObject<Actor>();
	Reflecting->SetModel("Cube");
	Reflecting->SetLocation(Vector(2.5f, 0.5f, 0.0f));
	Reflecting->GetModel()->SetMaterial(0, RI->LoadMaterialByName("Shaders/metal"));*/

	/*Domain = SpawnObject<Actor>();
	Domain->SetModel("ground");
	Domain->GetModel()->SetMaterial(0, RI->LoadMaterialByName("Shaders/test"));
	Domain->SetLocation(Vector(0.0, 0.0, 4.5));

	Pillars = SpawnObject<Actor>();
	Pillars->SetModel("pillars");
	Pillars->GetModel()->SetMaterial(0, RI->LoadMaterialByName("Shaders/pillars"));
	Pillars->GetModel()->SetMaterial(1, RI->LoadMaterialByName("Shaders/rocks"));
	Pillars->SetLocation(Vector(0.0, 0.0, -3.0));
	Pillars->SetScale(Vector(3.f));*/

	float size = 80.f;

	//for (int x = 0; x < 100; x++) {
	//	VisibleObject* next = SpawnObject<VisibleObject>();
	//	next->SetModel("Cube");
	//	float rx = (float)rand() / (float)RAND_MAX - 0.5f;
	//	float ry = (float)rand() / (float)RAND_MAX - 0.5f;
	//	float rz = (float)rand() / (float)RAND_MAX;
	//	//float scale = (float)rand() / (float)RAND_MAX * 2.f;
	//	//next->SetScale(Vector(scale));
	//	next->GetModel()->SetMaterial(0, RI->LoadMaterialByName("Shaders/rocks"));
	//	next->SetLocation(Vector(rx * size, ry * size, rz * size / 3));
	//	Spheres[x] = next;
	//}

	//Spheres[10]->SetScale(2.f);

	/*for (int x = 0; x < 20; x++) {
		for (int y = 0; y < 20; y++) {
			Light* next = SpawnObject<Light>();
			next->Data.Type = 1;
			float rx = (float)rand() / (float)RAND_MAX - 0.5f;
			float ry = (float)rand() / (float)RAND_MAX - 0.5f;
			float rz = (float)rand() / (float)RAND_MAX;
			float scale = (float)rand() / (float)RAND_MAX * 2.f;
			next->Data.Size = 4.f;
			next->Data.Intensity = 20.f;
			next->Data.Location = Vector(rx * size, ry * size, rz * size / 3);
			next->Disable();
			Lights.push_back(next);
		}
	}*/
}
