#include "Interface/IRender.h"
#include "TestPlayerLoad.h"
#include "Objects/MovementComponent.h"
#include "Objects/Terrain.h"
#include "Timer.h"
#include "Objects/InstancedObject.h"
#include "TestUILoad.h"
#include "PauseUI.h"
#include <Interface/WindowManager.h>
#include <GamePlay/Scene.h>
#include "Objects/InputComponent.h"

void TestPlayerLoad::OpenConsole(bool) {
	Console::Create();
}

void TestPlayerLoad::UseCursor(bool keydown)
{
	if (keydown && pause == nullptr) {
		WindowManager::SetShowCursor(0, cursorState);
		cursorState = !cursorState;
	}
}

TestPlayerLoad::TestPlayerLoad() : Player()
{
	mouseSens = 0.5f;
	Speed = 5.f;
	InputMode = true;
	cursorState = true;
	spawnCounter = 0;

	//Reqister used Inputs
	
	//Player Model
	Mesh = SpawnObject<VisibleObject>();
	Mesh->SetModel("Cube");
	Mesh->GetModel()->SetAABB(AABB(Vector(-0.5f), Vector(0.5f)));

	//Player Movement
	Movement = SpawnObject<MovementComponent>();
	Movement->SetTarget(dynamic_cast<Actor*>(this));
	Movement->SetGravity(true);

	//Skybox
	Sky = SpawnObject<VisibleObject>();
	Sky->SetModel(MI->LoadData(Sky, "SkySphere"));
	Sky->GetModel()->SetMaterial(0, RI->LoadMaterialByName("Assets/Materials/Sky"));
	Sky->SetScale(Sky->GetScale() * 2.0f);

	//Testing UI
	auto ui = SpawnObject<TestUILoad>();
	UI::AddToScreen(ui, this);

	pause = nullptr;
}

void TestPlayerLoad::RegisterInputs(InputComponent* com)
{
	com->RegisterKeyContinuousInput(81, &TestPlayerLoad::RunInputQ, this);
	com->RegisterKeyContinuousInput(90, &TestPlayerLoad::RunInputZ, this);
	com->RegisterKeyContinuousInput(87, &TestPlayerLoad::RunInputW, this);
	com->RegisterKeyContinuousInput(65, &TestPlayerLoad::RunInputA, this);
	com->RegisterKeyContinuousInput(83, &TestPlayerLoad::RunInputS, this);
	com->RegisterKeyContinuousInput(68, &TestPlayerLoad::RunInputD, this);
	com->RegisterKeyInput(32, &TestPlayerLoad::RunInputSpace, this);
	com->RegisterKeyInput(340, &TestPlayerLoad::RunInputShift, this);
	com->RegisterKeyInput(0, &TestPlayerLoad::LeftMouseDown, this);
	com->RegisterKeyInput(49, &TestPlayerLoad::InputOne, this);
	com->RegisterKeyInput(50, &TestPlayerLoad::InputTwo, this);
	com->RegisterKeyInput(256, &TestPlayerLoad::InputExit, this);
	com->RegisterKeyInput(257, &TestPlayerLoad::OpenConsole, this);
	com->RegisterMouseInput(0, &TestPlayerLoad::MouseMoved, this);
	com->RegisterKeyInput(69, &TestPlayerLoad::UseCursor, this);
}

//Handle Inputs
void TestPlayerLoad::RunInputQ(float delta, bool KeyDown)
{
	Vector dir(0.0,0.0,1.0);
	Movement->AddInput(dir.Normalize());
}
void TestPlayerLoad::RunInputZ(float delta, bool KeyDown)
{
	Vector dir(0.0, 0.0, -1.0);
	Movement->AddInput(dir.Normalize());
}

void TestPlayerLoad::RunInputW(float delta, bool KeyDown)
{
	Vector dir = -GetCamera()->GetForwardVector();
	dir.Z = 0.f;
	Movement->AddInput(dir.Normalize());
}

void TestPlayerLoad::RunInputA(float delta, bool KeyDown)
{
	Movement->AddInput(-GetCamera()->GetRightVector());
}

void TestPlayerLoad::RunInputD(float delta, bool KeyDown)
{
	Movement->AddInput(GetCamera()->GetRightVector());
}

void TestPlayerLoad::RunInputS(float delta, bool KeyDown)
{
	Vector dir = GetCamera()->GetForwardVector();
	dir.Z = 0.f;
	Movement->AddInput(dir.Normalize());
}

void TestPlayerLoad::RunInputSpace(bool KeyDown)
{
	if (!Movement->IsInAir() && KeyDown)
		Movement->AddImpulse(Vector(0.f, 0.f, 300.f));
}

void TestPlayerLoad::InputOne(bool KeyDown)
{
	if (KeyDown)
		InputMode = !InputMode;
}

void TestPlayerLoad::InputTwo(bool KeyDown)
{
	if (KeyDown)
		InputMode = !InputMode;
	Scene::OpenLevel("Assets/Maps/test");
}

void TestPlayerLoad::RunInputShift(bool KeyDown)
{
	if (KeyDown) Movement->SetMaxSpeed(10.f);
	else Movement->SetMaxSpeed(5.f);
}


void TestPlayerLoad::LeftMouseDown(bool)
{
}

void TestPlayerLoad::RightMouseDown(bool KeyDown)
{

}

void TestPlayerLoad::MouseMoved(float X, float Y)
{
	const Vector& rot = Rotation;
	if (cursorState) SetRotation(Vector(rot.X + X * mouseSens, rot.Y + Y * mouseSens < 89.f && rot.Y + Y * mouseSens > -89.f ? rot.Y + Y * mouseSens : rot.Y, rot.Z));
}

void TestPlayerLoad::InputExit(bool down)
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

void TestPlayerLoad::Tick(float)
{
	GetCamera()->SetLocation(Location + Vector(0.f, 0.f, 1.5f));
	GetCamera()->SetRotation(Rotation);
	Sky->SetLocation(Location);
}

void TestPlayerLoad::BeginPlay()
{
	Console::Log("Hello beautiful world");
}

void TestPlayerLoad::OnDestroyed()
{
}
