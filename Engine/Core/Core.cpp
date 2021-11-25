#include <iostream>
#include "GameLoop.h"
#include "Core.h"


GameLoop* Loop = nullptr;
INISettings* INI = nullptr;

void Exit()	
{
	Loop->Quit();
}

BaseObject* SpawnObject(String type)
{
	if (ObjectManager::TypeList().find(type) == ObjectManager::TypeList().end()) return nullptr;
	return ObjectManager::TypeList()[type]("", 0, Constants::Record::SPAWNED, false, 0);
}

GameState* GetGameState() {
	return GameLoop::State;
}

BaseObject* FindObjectByRecord(const RecordInt& record)
{
	return ObjectManager::GetByRecord<BaseObject>(record);
}
