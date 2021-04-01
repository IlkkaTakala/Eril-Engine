#include <iostream>
#include "GameLoop.h"
#include "Core.h"

GameLoop* Loop = nullptr;
INISettings* INI = nullptr;

void Exit()	
{
	Loop->Quit();
}

