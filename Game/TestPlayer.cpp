#include "IRender.h"
#include "TestPlayer.h"

TestPlayer::TestPlayer() : Player()
{
	mouseSens = 0.5f;
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

void TestPlayer::RunInputW(bool KeyDown)
{
	GetCamera()->SetLocation(GetCamera()->GetLocation() - GetCamera()->GetForwardVector() * 0.1f);
}

void TestPlayer::RunInputA(bool KeyDown)
{
	GetCamera()->SetLocation(GetCamera()->GetLocation() - GetCamera()->GetRightVector() * 0.1f);
}

void TestPlayer::RunInputD(bool KeyDown)
{
	GetCamera()->SetLocation(GetCamera()->GetLocation() + GetCamera()->GetRightVector() * 0.1f);
}

void TestPlayer::RunInputS(bool KeyDown)
{
	GetCamera()->SetLocation(GetCamera()->GetLocation() + GetCamera()->GetForwardVector() * 0.1f);
}

void TestPlayer::RunInputSpace(bool KeyDown)
{
	
}

void TestPlayer::RunInputShift(bool KeyDown)
{
	
}


void TestPlayer::LeftMouseDown(bool)
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

	Domain = SpawnObject<Actor>();
	Domain->SetModel("Buildings");
	Domain->SetScale(Vector(0.5, 0.5, 0.5));
	Domain->SetLocation(Vector(-0.5, 0.0, 0.0));

	Domain = SpawnObject<Actor>();
	Domain->SetModel("Buildings");
	Domain->SetScale(Vector(0.5, 0.5, 0.5));
	Domain->SetLocation(Vector(0.5, -1.0, 0.5));

	Domain = SpawnObject<Actor>();
	Domain->SetModel("Buildings");
	Domain->SetScale(Vector(0.5, 0.5, 0.5));
	Domain->SetLocation(Vector(0.5, 0.0, 0.0));

	/*Domain = SpawnObject<VisibleObject>();
	Domain->SetModel("sphere");
	Domain->SetScale(Vector(0.5, 0.5, 0.5));
	Domain->SetLocation(Vector(0.5, -1.0, 0.0));*/

}
