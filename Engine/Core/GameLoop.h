#pragma once
#include "Core.h"
#include <mutex>
#include <vector>
#include <list>

class GameState;
class GC;
class Input;
class ECSWorldManager;

class GameLoop
{
	friend class GC;
public:
	GameLoop();
	~GameLoop();
	int Start();
	void Quit();
	void AddToTick(Tickable* t) { std::unique_lock<std::mutex> lock(TickListMutex); TickList.push_back(t); }
	void RemoveFromTick(Tickable* t) { std::unique_lock<std::mutex> lock(TickListMutex); TickListRemoval.push_back(t); }
	void ClearTick() { std::unique_lock<std::mutex> lock(TickListMutex); TickList.clear(); TickListRemoval.clear(); }
	static Ref<GameState> State;
	static Ref<Scene> World;

private:
	int MainLoop();

	static std::list<Tickable*> TickList;
	static std::list<Tickable*> TickListRemoval;

	GC* Collector;
	std::mutex TickListMutex;
	bool bQuit;
	bool bQuitStarted;
	float fps;
};