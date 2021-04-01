#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NOCOMM
#include <Windows.h>
#include "Objects/Actor.h"
#include "Gameplay/GameState.h"
#include "Gameplay/InputControl.h"
#include "../Game/EngineInterface.h"
#include "Settings.h"
#include "Renderer.h"
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
	if (InputHandler != nullptr) delete InputHandler;
	if (Collector != nullptr) delete Collector;
	delete INI;
}

int GameLoop::Start()
{
	Loop = this;
	INI = new INISettings("Settings.ini");
	if (!INI->IsValid()) return 10;

	RI::SetupWindow();

	Sleep(100);
	InputHandler = new Input();
	Collector = new GC(INI->GetValue("Engine", "DataFolder"));
	State = EngineInterface::CreateDefaults();

	return MainLoop();
}

void GameLoop::Quit()
{
	bQuitStarted = true;
	Collector->Quit();
	bQuit = true;
}

int GameLoop::MainLoop()
{
	std::chrono::duration<float> duration = std::chrono::milliseconds(0);
	auto begin = std::chrono::steady_clock::now();
	auto time = std::chrono::milliseconds(10);
	while (!bQuit) {
		auto start = std::chrono::steady_clock::now();

		std::this_thread::sleep_until(start + time);

		RI::Update();

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

		RI::Render();

		std::unique_lock<std::mutex> lock(TickListMutex);
		for (Tickable* t : TickListRemoval) {
			TickList.remove(t);
		}
		lock.unlock();
		duration = std::chrono::steady_clock::now() - start;
		fps = 1.f / duration.count();
	}
	
	RI::CleanRenderer();

	return 0;
}
