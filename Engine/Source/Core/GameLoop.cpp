#include "Objects/Actor.h"
#include "Gameplay/GameState.h"
#include "EngineInterface.h"
#include "Settings.h"
#include <Interface/IRender.h>
#include <Interface/AssetManager.h>
#include <Interface/AudioManager.h>
#include <Interface/IECS.h>
#include "GameLoop.h"
#include "WinConsole.h"
#include "GarbageCollector.h"
#include "Physics.h"
#include "Physics/BulletPhysics.h"
#include "Timer.h"
#include <GamePlay/Scene.h>

#ifdef USE_SCRIPTCORE
#include <ScriptCore.h>
#endif

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
	delete Collector;
	IECS::Destroy();
	AudioManager::Destroy();
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
		AssetManager::StartLoader();
		int x = std::atoi(INI->GetValue("Render", "ResolutionX").c_str());
		int y = std::atoi(INI->GetValue("Render", "ResolutionY").c_str());
		IRender::SetupWindow(x, y);
		II->SetInputHandler();
	}
	catch (const std::exception& e)
	{
		Console::Log(e.what());
		IRender::DestroyWindow();
		return 11;
	}
	AudioManager::Init();

	Console::Log("Creating defaults...\n");
	//Physics::init();
	EngineInterface::CreateDefaults();
	Collector = new GC();
	Console::Log("Loading finished");
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
	auto target = std::chrono::milliseconds(10);
	IRender::GameStart();
	while (!bQuit) {
		auto start = std::chrono::steady_clock::now();

		IRender::Update();

		II->ProcessInputs(duration.count());

		ObjectManager::UpdateLifetimes(duration.count());
		for (Tickable* t : TickList) {
			bool found = false;
			for (Tickable* old : TickListRemoval) {
				if (old == t) {
					found = true; break;
				}
			}
			if (found) continue;
			if (auto it = dynamic_cast<BaseObject*>(t); it && !it->IsActive())
				continue;
			t->Tick(duration.count());
		}
		Console::Evaluate();

		SystemsManager* ECSWorldSystemsManager = IECS::GetSystemsManager();
		ECSWorldSystemsManager->UpdateSystems(duration.count());

		Timer::UpdateTimers(duration.count());
		if (Physics::GetWorld() != nullptr) {
			Physics::CheckCollisions(duration.count());
		}

		IRender::Render(duration.count());

		ObjectManager::DeleteListed();
		//MI->ClearUnused(); // TODO

		std::unique_lock<std::mutex> lock(TickListMutex);
		for (Tickable* t : TickListRemoval) {
			TickList.remove(t);
		}
		TickListRemoval.clear();
		lock.unlock();

		//auto time = target - std::chrono::duration<double>(std::chrono::steady_clock::now() - start);
		//std::this_thread::sleep_for(time);
		Scene::CheckShouldLoad();

		duration = std::chrono::steady_clock::now() - start;
		fps = 1.f / duration.count();
		//Console::Log(std::to_string(total += duration.count()));
	}
	Collector->Quit();

	TickList.clear();
	
	IRender::CleanRenderer();

	return 0;
}
