#include <iostream>
#include "GameLoop.h"
#include "Core.h"
#include <Gameplay/Scene.h>
#ifdef USE_SCRIPTCORE
#include <ScriptCore.h>
namespace ScriptFunctions {
	int64 CreateObject(String* name, int id = 0) {
		if (ObjectManager::TypeList().find(*name) == ObjectManager::TypeList().end()) return 0;
		Console::Log("Spawning Object: " + *name);
		return ObjectManager::TypeList()[*name]("", id, Constants::Record::CONSOLE, false, 0)->GetRecord();
	}
	REGISTER_FUNCTION(CreateObject, global, 2);
}
#endif // SCRIPTCORE


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

void helpers::SpawnHelper(BaseObject* obj, const String& args)
{
	obj->LoadWithParameters(args);
	if (!Scene::isLoading()) obj->BeginPlay();
	auto t = dynamic_cast<Tickable*>(obj);
	ObjectManager::AddTick(t);
}
