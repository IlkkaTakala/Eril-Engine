#include "IRender.h"
#include "LightData.h"
#include "TestPlayer.h"

TestPlayer::TestPlayer() : Player()
{
	mouseSens = 0.5f;
	Speed = 5.f;
	LightMode = false;
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
}

void TestPlayer::RunInputW(float delta, bool KeyDown)
{
	GetCamera()->SetLocation(GetCamera()->GetLocation() - GetCamera()->GetForwardVector() * delta * Speed);
}

void TestPlayer::RunInputA(float delta, bool KeyDown)
{
	GetCamera()->SetLocation(GetCamera()->GetLocation() - GetCamera()->GetRightVector() * delta * Speed);
}

void TestPlayer::RunInputD(float delta, bool KeyDown)
{
	GetCamera()->SetLocation(GetCamera()->GetLocation() + GetCamera()->GetRightVector() * delta * Speed);
}

void TestPlayer::RunInputS(float delta, bool KeyDown)
{
	GetCamera()->SetLocation(GetCamera()->GetLocation() + GetCamera()->GetForwardVector() * delta * Speed);
}

void TestPlayer::RunInputSpace(float delta, bool KeyDown)
{
	
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
	if (KeyDown) Speed = 10.f;
	else Speed = 5.f;
}


void TestPlayer::LeftMouseDown(float delta, bool)
{

}

void TestPlayer::RightMouseDown(float delta, bool KeyDown)
{

}

void TestPlayer::MouseMoved(float X, float Y)
{
	const Vector& rot = GetCamera()->GetRotation();
	GetCamera()->SetRotation(Vector(rot.X + X * mouseSens, rot.Y + Y * mouseSens < 89.f && rot.Y + Y * mouseSens > -89.f ? rot.Y + Y * mouseSens : rot.Y, rot.Z));
}

void TestPlayer::Tick(float)
{
		
}

#include "Objects/InstancedObject.h"
#include "Objects/Actor.h"


void TestPlayer::BeginPlay()
{
	LightMode = true;

	DirLight = SpawnObject<Light>();
	DirLight->Data.Location = Vector(2.5f, 2.5f, 10.f);
	DirLight->Data.Type = LIGHT_DIRECTIONAL;
	DirLight->Data.Size = 20.0;
	DirLight->Data.Intensity = 5.0;
	DirLight->Data.Rotation = Vector(45.0, 0.0, 0.0);

	/*Reflecting = SpawnObject<InstancedObject>();
	Reflecting->SetModel("Cube");
	Reflecting->SetLocation(Vector(2.5f, 0.5f, 0.0f));
	Reflecting->GetModel()->SetMaterial(0, RI->LoadMaterialByName("Shaders/metal"));
	RecordInt r = 0x0010000000001111u;

	printf("Record: %i\n", r.GetModID());

	constexpr int count = 2000;
	Transformation arr[count];
	for (int i = 0; i < count; i++)
	{
		arr[i].Location = Vector(rand() % 200 - 100.f, rand() % 200 - 100.f, rand() % 200 - 100.f);
		arr[i].Scale = Vector(1.f, 1.f, 1.f);
	}

	Reflecting->AddInstances(count, arr);*/

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

	for (int x = 0; x < 100; x++) {
		Actor* next = SpawnObject<Actor>();
		next->SetModel("Cube");
		float rx = (float)rand() / (float)RAND_MAX - 0.5f;
		float ry = (float)rand() / (float)RAND_MAX - 0.5f;
		float rz = (float)rand() / (float)RAND_MAX;
		//float scale = (float)rand() / (float)RAND_MAX * 2.f;
		//next->SetScale(Vector(scale));
		next->GetModel()->SetMaterial(0, RI->LoadMaterialByName("Shaders/rocks"));
		next->SetLocation(Vector(rx * size, ry * size, rz * size / 3));
		Spheres[x] = next;
	}

	Spheres[10]->SetScale(2.f);

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
