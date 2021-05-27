#include "IRender.h"
#include "LightData.h"
#include "TestPlayer.h"

TestPlayer::TestPlayer() : Player()
{
	mouseSens = 0.5f;
	Speed = 5.f;
	//GetCamera()->SetLocation(INI->GetValue("Player", "Start"));
	//GetCamera()->SetRotation(INI->GetValue("Player", "Direction"));

	II->RegisterKeyInput(87, &TestPlayer::RunInputW, this);
	II->RegisterKeyInput(65, &TestPlayer::RunInputA, this);
	II->RegisterKeyInput(83, &TestPlayer::RunInputS, this);
	II->RegisterKeyInput(68, &TestPlayer::RunInputD, this);
	II->RegisterKeyInput(32, &TestPlayer::RunInputSpace, this);
	II->RegisterKeyInput(340, &TestPlayer::RunInputShift, this);
	II->RegisterKeyInput(0x01, &TestPlayer::LeftMouseDown, this);
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

void TestPlayer::RunInputShift(float delta, bool KeyDown)
{
	if (KeyDown) Speed = 10.f;
	else Speed = 5.f;
}


void TestPlayer::LeftMouseDown(float delta, bool)
{
	/*Vector Start(CameraPoint);
	Vector End(CameraPoint + CameraDirection * 100);
	Vector Out;
	VisibleObject* Hit = nullptr;
	Trace(Start, End, Out, &Hit);
	if (Hit != nullptr) Hit->DestroyObject();*/
}

void TestPlayer::MouseMoved(float X, float Y)
{
	const Vector& rot = GetCamera()->GetRotation();
	GetCamera()->SetRotation(Vector(rot.X + X * mouseSens, rot.Y + Y * mouseSens < 89.f && rot.Y + Y * mouseSens > -89.f ? rot.Y + Y * mouseSens : rot.Y, rot.Z));
}

void TestPlayer::Tick(float)
{
		
}

#include "Objects/Actor.h"

void TestPlayer::BeginPlay()
{

	Light* nexta = SpawnObject<Light>();
	nexta->Data.Location = Vector(1.5f, 1.5f, 1.f);
	nexta->Data.Type = 0;
	nexta->Data.Rotation = Vector(45.0, 45.0, 0.0);

	/*nexta = SpawnObject<Light>();
	nexta->Data.Location = Vector(-1.5f, -1.5f, 3.f);
	nexta->Data.Type = 1;

	Light* nexta = SpawnObject<Light>();
	nexta->Data.Location = Vector(-1.5f, -1.5f, 3.f);
	nexta->Data.Rotation = Vector(45.0, 45.0, 0.0);
	nexta->Data.Type = 0;*/

	/*Domain = SpawnObject<Actor>();
	Domain->SetModel("sphere");
	Domain->GetModel()->SetMaterial(0, RI->LoadMaterialByName("Shaders/for"));
	Domain->SetScale(Vector(0.5, 0.5, 0.5));
	Domain->SetLocation(Vector(0, 9.0, 0.f));*/

	Actor* next = SpawnObject<Actor>();
	next->SetModel("sphere");
	next->SetLocation(Vector(1.5f, 0.5f, 1.f));

	Domain = SpawnObject<Actor>();
	Domain->SetModel("room");
	//Domain->SetScale(Vector(0.5, 0.5, 0.5));
	Domain->SetLocation(Vector(0.0, 0.0, 0.0));

	/*Domain = SpawnObject<Actor>();
	Domain->SetModel("Buildings");
	Domain->SetScale(Vector(0.5, 0.5, 0.5));
	Domain->SetLocation(Vector(0.5, -1.0, 0.5));

	Domain = SpawnObject<Actor>();
	Domain->SetModel("sphere");
	Domain->GetModel()->SetMaterial(0, RI->LoadMaterialByName("Shaders/for"));
	Domain->SetScale(Vector(0.5, 0.5, 0.5));
	Domain->SetLocation(Vector(0.5, 0.0, 0.0));

	/*Domain = SpawnObject<VisibleObject>();
	Domain->SetModel("sphere");
	Domain->SetScale(Vector(0.5, 0.5, 0.5));
	Domain->SetLocation(Vector(0.5, -1.0, 0.0));*/

}
