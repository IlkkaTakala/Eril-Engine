#pragma once
#include "Core.h"
#include <mutex>
#include <vector>
#include <list>

class GameState;
class GC;
class Input;

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

private:
	int MainLoop();

	static std::list<Tickable*> TickList;
	static std::list<Tickable*> TickListRemoval;

	GC* Collector;
	std::mutex TickListMutex;
	static Ref<GameState> State;
	bool bQuit;
	bool bQuitStarted;
	float fps;
};