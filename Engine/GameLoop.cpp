#include "Objects/Actor.h"
#include "Gameplay/GameState.h"
#include "../Game/EngineInterface.h"
#include "Settings.h"
#include "IRender.h"
#include "GameLoop.h"

using namespace std;

std::list<Tickable*> GameLoop::TickList;
std::list<Tickable*> GameLoop::TickListRemoval;
Ref<GameState> GameLoop::State = nullptr;

GameLoop::GameLoop()
{
	bQuit = false;
	bQuitStarted = false;
	InputHandler = nullptr;
	Collector = nullptr;
	fps = 0.f;
}

GameLoop::~GameLoop()
{
	if (State != nullptr) State == nullptr;
	if (Collector != nullptr) delete Collector;
	delete INI;
	delete RI;
	delete MI;
	delete II;
}

int GameLoop::Start()
{
	Loop = this;
	INI = new INISettings("Settings.ini");
	if (!INI->IsValid()) return 10;

	try
	{
		RI->SetupWindow(640, 480);
		RI->LoadShaders();
		II->SetInputHandler();
		MI->StartLoading();
	}
	catch (const std::exception& e)
	{
		printf(e.what());
		return 11;
	}
	
	//Collector = new GC();
	State = EngineInterface::CreateDefaults();

	return MainLoop();
}

void GameLoop::Quit()
{
	bQuitStarted = true;
	if (Collector != nullptr) Collector->Quit();
	bQuit = true;
}

int GameLoop::MainLoop()
{
	std::chrono::duration<float> duration = std::chrono::milliseconds(0);
	auto begin = std::chrono::steady_clock::now();
	auto time = std::chrono::milliseconds(10);
	while (!bQuit) {
		auto start = std::chrono::steady_clock::now();

		//std::this_thread::sleep_until(start + time);

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

		RI->Render();

		std::unique_lock<std::mutex> lock(TickListMutex);
		for (Tickable* t : TickListRemoval) {
			TickList.remove(t);
		}
		lock.unlock();
		duration = std::chrono::steady_clock::now() - start;
		fps = 1.f / duration.count();
		printf("\r%c[2K", 27);
		printf("FPS: %.2f", fps);
	}
	
	RI->CleanRenderer();

	return 0;
}
