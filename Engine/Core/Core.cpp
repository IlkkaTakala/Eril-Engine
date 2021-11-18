#include <iostream>
#include "GameLoop.h"
#include "Core.h"


GameLoop* Loop = nullptr;
INISettings* INI = nullptr;

void Exit()	
{
	Loop->Quit();
}

bool helpers::helper(BaseObject* base, const String& args)
{
	if (base == nullptr) {
		return false;
	}
	base->LoadWithParameters(args);
	base->BeginPlay();
	auto t = dynamic_cast<Tickable*>(base);
	ObjectManager::AddTick(t);
	return true;
}

BaseObject* SpawnObject(String type)
{
	if (ObjectManager::TypeList.find(type) == ObjectManager::TypeList.end()) return nullptr;
	return ObjectManager::TypeList[type]("", 0, Constants::Record::SPAWNED, false, 0);
}

GameState* GetGameState() {
	return GameLoop::State;
}