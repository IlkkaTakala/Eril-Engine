#include "Objects/Actor.h"
#include "Gameplay/GameState.h"
#include "../Game/EngineInterface.h"
#include "Settings.h"
#include "IRender.h"
#include "GameLoop.h"
#define	WIN32_LEAN_AND_MEAN
#include <Windows.h>

using namespace std;

std::list<Tickable*> GameLoop::TickList;
std::list<Tickable*> GameLoop::TickListRemoval;
Ref<GameState> GameLoop::State = nullptr;

GameLoop::GameLoop()
{
	bQuit = false;
	bQuitStarted = false;
	fps = 0.f;
}

GameLoop::~GameLoop()
{
	if (State != nullptr) State == nullptr;
	if (GC::Pointers.size() > 0) {
		for (auto const& i : GC::Pointers) {
			delete i.second;
		}
	}
	GC::Pointers.clear();
	delete INI;
	delete II;
	delete MI;
	delete RI;
}

int GameLoop::Start()
{
	Loop = this;
	INI = new INISettings("Settings.ini");
	if (!INI->IsValid()) return 10;

	try
	{
		if (INI->GetValue("Engine", "Console").c_str() == "true") AllocConsole();
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
	
	State = EngineInterface::CreateDefaults();

	return MainLoop();
}

void GameLoop::Quit()
{
	bQuitStarted = true;
	bQuit = true;
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
