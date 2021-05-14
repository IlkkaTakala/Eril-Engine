#include <iostream>
#include "GameLoop.h"
#include "Core.h"

GameLoop* Loop = nullptr;
INISettings* INI = nullptr;

void Exit()	
{
	Loop->Quit();
}

Data* GetObjectByName(const String& name)
{
	auto result = GC::Pointers.find(name);
	return result == GC::Pointers.end() ? nullptr : result->second;
}

