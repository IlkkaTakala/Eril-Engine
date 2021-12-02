#include "Interface/IRender.h"
#include "LightData.h"
#include "TestPlayer.h"
#include "Objects/MovementComponent.h"
#include "Objects/Terrain.h"
#include "Timer.h"
#include "Objects/InstancedObject.h"
#include "Hunter.h"
#include "ECSTesting.h"
#include "TestUI.h"
#include <Interface/WindowManager.h>

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
	//ECS TEST
	//ecsTest = SpawnObject<ECSTesting>();

	mouseSens = 0.5f;
	Speed = 5.f;
	InputMode = true;
	spawnCounter = 0;
	//GetCamera()->SetLocation(INI->GetValue("Player", "Start"));
	//GetCamera()->SetRotation(INI->GetValue("Player", "Direction"));

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

	Mesh = SpawnObject<VisibleObject>();
	Mesh->SetModel("Cube");
	Mesh->GetModel()->SetAABB(AABB(Vector(-0.5f), Vector(0.5f)));


	Movement = SpawnObject<MovementComponent>();
	Movement->SetTarget(dynamic_cast<Actor*>(this));
	Movement->SetGravity(true);

	Sky = SpawnObject<VisibleObject>();
	Sky->SetModel(MI->LoadData(Sky, "SkySphere"));
	Sky->GetModel()->SetMaterial(0, RI->LoadMaterialByName("Assets/Materials/Sky"));
	
	/*
	{
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
		
		Trees2 = SpawnObject<InstancedObject>();
		Trees2->SetModel(MI->LoadData(Trees2, "tree2"));
		Trees2->GetModel()->SetMaterial(0, RI->LoadMaterialByName("Assets/Materials/tree"));
		Trees2->GetModel()->SetMaterial(1, RI->LoadMaterialByName("Assets/Materials/leaves2"));
		Trees2->GetModel()->SetAABB(AABB(Vector(-100.f), Vector(100.f)));

		for (int i = 0; i < count; i++)
		{
			float x = rand() % 100 - 50.f;
			float y = rand() % 100 - 50.f;
			float s = (1.f - rand() / (float)RAND_MAX * 0.7f) * 1.5f;
			arr[i].Location = Vector(x, y, terra[0]->GetHeight(x, y) - 0.2f);
			arr[i].Scale = Vector(s);
		}
		Trees2->AddInstances(count, arr);
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
			float s = (1.f - rand() / (float)RAND_MAX * 0.7f) * 1.5f;
			Vector normal = terra[0]->GetNormal(x, y);
			arr[i].Location = Vector(x, y, terra[0]->GetHeight(x, y)) - normal * 0.1;
			arr[i].Scale = Vector(1.f);
			Vector up(0.f, 0.f, 1.f);
			Vector axis = Vector::Cross(normal, up).Normalize();
			float angle = acos(Vector::Dot(up, normal));
			arr[i].Rotation = Vector::toEuler(axis, angle) * 180.f / PI;
		}
		Grass->AddInstances(count, arr);
		delete[] arr;

		Flowers = SpawnObject<InstancedObject>();
		Flowers->SetModel(MI->LoadData(Flowers, "flower"));
		Flowers->GetModel()->SetMaterial(0, RI->LoadMaterialByName("Assets/Materials/flower"));
		Flowers->GetModel()->SetAABB(AABB(Vector(-100.f), Vector(100.f)));

		count = 1000;
		arr = new Transformation[count]();
		for (int i = 0; i < count; i++)
		{
			float x = rand() % 100 - 50.f;
			float y = rand() % 100 - 50.f;
			float s = (1.f - rand() / (float)RAND_MAX * 0.4f);
			Vector normal = terra[0]->GetNormal(x, y);
			arr[i].Location = Vector(x, y, terra[0]->GetHeight(x, y));
			arr[i].Scale = Vector(s);
			Vector up(0.f, 0.f, 1.f);
			Vector axis = Vector::Cross(normal, up).Normalize();
			float angle = acos(Vector::Dot(up, normal));
			arr[i].Rotation = Vector::toEuler(axis, angle) * 180.f / PI;
		}
		Flowers->AddInstances(count, arr);

		delete[] arr;
	}
	*/

	//hunt = SpawnObject<Hunter>();
	//hunt->SetLocation(Vector(100.f, 0.f, 0.f));
	//hunt->move->SetGround(terra[0]);

	//auto ui = SpawnObject<TestUI>();
	//UI::AddToScreen(ui, this);
}

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

void TestPlayer::Tick(float)
{
	GetCamera()->SetLocation(Location + Vector(0.f, 0.f, 1.5f));
	GetCamera()->SetRotation(Rotation);
	Sky->SetLocation(Location);
}

void TimeFunction(float d) {
}

void TestPlayer::BeginPlay()
{
	/*DirLight = SpawnObject<Light>();
	DirLight->Data.Location = Vector(0.f, 0.f, 1.f);
	DirLight->Data.Type = LIGHT_POINT;
	DirLight->Data.Size = 3.f;
	DirLight->Data.Intensity = 5.f;
	DirLight->Data.Color = Vector(1.f);
	DirLight->Data.Rotation = Vector(0.0, 45.0, -45.0);*/

		printf("Spawned object\n");
	Timer::CreateTimer(5.f, TimeFunction, false);

	

	uint64 l = 0xABCDEF0123456789;
	uint32 h = (uint32)l;
	printf("0x%lx\n", h);
	
	/*
	Lights[0] = SpawnObject<Light>();

	Lights[0]->Data.Location = Vector(70, 70, 1.2f);
	Lights[0]->Data.Type = LIGHT_POINT;
	Lights[0]->Data.Size = 80.f;
	Lights[0]->Data.Intensity = 50.f;
	Lights[0]->Data.Color = Vector(1.f);
	*/
	
	Terrain* terrain = ObjectManager::GetByRecord<Terrain>(0xA0005554);

	for (int i = 1; i < 1; i++) {

		float x = rand() % 100;
		float y = rand() % 100;
		//float s = 1.f - rand() / (float)RAND_MAX * 0.7f;

		Lights[i] = SpawnObject<Light>();

		Lights[i]->Data.Location = Vector(x, y, terrain->GetHeight(x,y) + 0.5f);
		Lights[i]->Data.Type = LIGHT_POINT;
		Lights[i]->Data.Size = 5.f;
		Lights[i]->Data.Intensity = rand() / (float)RAND_MAX * 20.f + 10;
		Lights[i]->Data.Color = Vector(1.f);
	}
	
	float x = 9;
	float y = 5;
	//float s = 1.f - rand() / (float)RAND_MAX * 0.7f;

	Lights[0] = SpawnObject<Light>();

	Lights[0]->Data.Location = Vector(x, y, terrain->GetHeight(x, y) + 0.5f);
	Lights[0]->Data.Type = LIGHT_POINT;
	Lights[0]->Data.Size = 15.f;
	Lights[0]->Data.Intensity = 14.0f;
	Lights[0]->Data.Color = Vector(1.0, 1.0, 1.0);

	/*
	Lights[1] = SpawnObject<Light>();

	Lights[1]->Data.Location = Vector(x, y, terrain->GetHeight(x, y) + 0.5f);
	Lights[1]->Data.Type = LIGHT_DIRECTIONAL;
	Lights[1]->Data.Size = 15.f;
	Lights[1]->Data.Intensity = 10.0f;
	Lights[1]->Data.Color = Vector(1.0, 1.0, 1.0);
	Lights[1]->Data.Rotation = Vector(0.5, 0.5, 0.5);
	*/
	

	Console::Log("Hello beautiful world");
}
