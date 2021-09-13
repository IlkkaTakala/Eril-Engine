#include "Objects/Actor.h"
#include "Gameplay/GameState.h"
#include "../Game/EngineInterface.h"
#include "Settings.h"
#include "IRender.h"
#include "GameLoop.h"
#include "WinConsole.h"
#include "GarbageCollector.h"

using namespace std;

std::list<Tickable*> GameLoop::TickList;
std::list<Tickable*> GameLoop::TickListRemoval;
Ref<GameState> GameLoop::State = nullptr;

GameLoop::GameLoop()
{
	bQuit = false;
	bQuitStarted = false;
	fps = 0.f;
	Collector = new GC();
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
}

int GameLoop::Start()
{
	printf("Starting game...\n");
	Loop = this;
	INI = new INISettings("Settings.ini");
	if (!INI->IsValid()) return 10;

	try
	{
		if (INI->GetValue("Engine", "Console") == "true") 
			AddConsole();
		MI->StartLoading();
		int x = std::atoi(INI->GetValue("Render", "ResolutionX").c_str());
		int y = std::atoi(INI->GetValue("Render", "ResolutionY").c_str());
		RI->SetupWindow(x, y);
		II->SetInputHandler();
	}
	catch (const std::exception& e)
	{
		printf(e.what());
		return 11;
	}
	
	printf("Creating defaults...\n");
	State = EngineInterface::CreateDefaults();

	printf("Loading finished\n");
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

		RI->Render(duration.count());

		std::unique_lock<std::mutex> lock(TickListMutex);
		for (Tickable* t : TickListRemoval) {
			TickList.remove(t);
		}
		lock.unlock();
		duration = std::chrono::steady_clock::now() - start;
		//fps = 1.f / duration.count();
	}

	TickList.clear();
	
	RI->CleanRenderer();

	return 0;
}
