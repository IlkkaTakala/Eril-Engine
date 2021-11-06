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

void TimeFunction(float d) {
	printf("Hello, timer ran\n");
}

void TestPlayer::BeginPlay()
{
	RecordInt r = GetRecord();//0xABCDEF0123456789;
	printf("Record: 0x%llx\n", (uint64)r);
	printf("Mod: 0x%x\n", (uint)r.GetModID());
	printf("State: 0x%x\n", (uint8)r.GetSpawnState());
	printf("Server: %s\n", r.GetIsServer() ? "true" : "false");

	if (r.GetSpawnState() == Constants::Record::SPAWNED) {
		printf("Spawned object\n");
	}
}
