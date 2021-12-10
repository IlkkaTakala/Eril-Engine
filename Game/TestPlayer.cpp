#include "IRender.h"
#include "LightData.h"
#include "TestPlayer.h"
#include "Objects/MovementComponent.h"
#include "Objects/Terrain.h"
#include "Timer.h"
#include "Objects/InstancedObject.h"
#include "Hunter.h"

TestPlayer::TestPlayer() : Player()
{
	mouseSens = 0.5f;
	Speed = 5.f;
	InputMode = true;
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

	Mesh = SpawnObject<VisibleObject>();
	Mesh->SetModel("Cube");
	Mesh->GetModel()->SetAABB(AABB(Vector(-0.5f), Vector(0.5f)));

	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 2; x++) {
			terra[y * 2 + x] = SpawnObject<Terrain>();
			terra[y * 2 + x]->InitTerrain(100, Vector(100.f, 100.f, 1.f), Vector(-100.f * x, -100.f * y, 0.f));
		}
	}
	Movement = SpawnObject<MovementComponent>();
	Movement->SetTarget(dynamic_cast<Actor*>(this));
	Movement->SetGravity(true);
	Movement->SetGround(terra[0]);

	Sky = SpawnObject<VisibleObject>();
	Sky->SetModel(MI->LoadData(Sky, "SkySphere"));
	Sky->GetModel()->SetMaterial(0, RI->LoadMaterialByName("Assets/Materials/Sky"));
	
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
		Trees->AddInstances(count, arr); // visible objects instead?
		
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

	box = SpawnObject<SceneComponent>();
	box->


	hunt = SpawnObject<Hunter>();
	hunt->SetLocation(Vector(100.f, 0.f, 0.f));
	hunt->move->SetGround(terra[0]);
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
	if (KeyDown)
		InputMode = !InputMode;
}

void TestPlayer::InputTwo(float delta, bool KeyDown)
{
	if (KeyDown)
		InputMode = !InputMode;
}

void TestPlayer::RunInputShift(float delta, bool KeyDown)
{
	if (KeyDown) Movement->SetMaxSpeed(10.f);
	else Movement->SetMaxSpeed(5.f);
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
}

#include "Objects/Actor.h"
#include "Objects/VisibleObject.h"

void TimeFunction(float d) {
	printf("Hello, timer ran\n");
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

	Timer::CreateTimer(5.f, TimeFunction, false);

	/*for (int i = 0; i < 100; i++) {

		float x = rand() % 100 - 50.f;
		float y = rand() % 100 - 50.f;
		float s = 1.f - rand() / (float)RAND_MAX * 0.7f;

		Lights[i] = SpawnObject<Light>();

		Lights[i]->Data.Location = Vector(x, y, terra[0]->GetHeight(x, y) + 0.2f);
		Lights[i]->Data.Type = LIGHT_POINT;
		Lights[i]->Data.Size = 5.f;
		Lights[i]->Data.Intensity = rand() / (float)RAND_MAX * 20.f;
		Lights[i]->Data.Color = Vector(1.f);
	}*/
}
