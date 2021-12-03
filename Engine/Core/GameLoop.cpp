#include "Objects/Actor.h"
#include "Gameplay/GameState.h"
#include "EngineInterface.h"
#include "Settings.h"
#include "Interface/IRender.h"
#include "GameLoop.h"
#include "WinConsole.h"
#include "GarbageCollector.h"
#include "Physics.h"
#include "Timer.h"
#include <GamePlay/Scene.h>
#include <Interface/IECS.h>

using namespace std;

std::list<Tickable*> GameLoop::TickList;
std::list<Tickable*> GameLoop::TickListRemoval;
Ref<GameState> GameLoop::State = nullptr;
Ref<Scene> GameLoop::World = nullptr;

GameLoop::GameLoop()
{
	bQuit = false;
	bQuitStarted = false;
	fps = 0.f;
	Collector = nullptr;
}

GameLoop::~GameLoop()
{
	if (State != nullptr) State == nullptr;
	ObjectManager::CleanObjects();
	delete INI;
	delete II;
	delete MI;
	delete RI;
	delete Collector;
	IECS::Destroy();
}

int GameLoop::Start()
{
	Console::Log("Starting game...");
	Loop = this;
	INI = new INISettings("Settings.ini");
	if (!INI->IsValid()) return 10;

	//Initialize ECS World
	IECS::Init();

	try
	{
		if (INI->GetValue("Engine", "Console") == "true") 
			Console::Create();
		MI->StartLoading();
		int x = std::atoi(INI->GetValue("Render", "ResolutionX").c_str());
		int y = std::atoi(INI->GetValue("Render", "ResolutionY").c_str());
		RI->SetupWindow(x, y);
		II->SetInputHandler();
	}
	catch (const std::exception& e)
	{
		Console::Log(e.what());
		RI->DestroyWindow();
		return 11;
	}

	
	
	Console::Log("Creating defaults...");
	State = EngineInterface::CreateDefaults();
	Collector = new GC();

	Console::Log("Loading finished");
	return MainLoop();
}

void GameLoop::Quit()
{
	bQuitStarted = true;
	bQuit = true;
	Collector->Quit();
}

int GameLoop::MainLoop()
{
	std::chrono::duration<float> duration = std::chrono::milliseconds(0);
	auto begin = std::chrono::steady_clock::now();
	auto time = std::chrono::milliseconds(10);
	RI->GameStart();
	while (!bQuit) {
		auto start = std::chrono::steady_clock::now();

		RI->Update();

		II->ProcessInputs(duration.count());

		for (Tickable* t : TickList) {
			bool found = false;
			for (Tickable* old : TickListRemoval) {
				if (old == t) {
					found = true; break;
				}
			}
			if (found) continue;
			t->Tick(duration.count());
		}

		SystemsManager* ECSWorldSystemsManager = IECS::GetSystemsManager();
		ECSWorldSystemsManager->UpdateSystems(duration.count());

		Timer::UpdateTimers(duration.count());
		Physics::CheckCollisions();

		RI->Render(duration.count());

		ObjectManager::DeleteListed();

		std::unique_lock<std::mutex> lock(TickListMutex);
		for (Tickable* t : TickListRemoval) {
			TickList.remove(t);
		}
		TickListRemoval.clear();
		lock.unlock();
		duration = std::chrono::steady_clock::now() - start;
		//fps = 1.f / duration.count();
	}

	TickList.clear();
	
	RI->CleanRenderer();

	return 0;
}
