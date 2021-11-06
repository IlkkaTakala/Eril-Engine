#include <iostream>
#include "GameLoop.h"
#include "Core.h"


GameLoop* Loop = nullptr;
INISettings* INI = nullptr;

void Exit()	
{
	Loop->Quit();
}

bool helpers::helper(BaseObject* base)
{
	if (base == nullptr) {
		return false;
	}
	base->BeginPlay();
	auto t = dynamic_cast<Tickable*>(base);
	ObjectManager::AddTick(t);
	return true;
}

GameState* GetGameState() {
	return GameLoop::State;
}