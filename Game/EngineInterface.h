#pragma once
#include "Core.h"

class GameState;

class EngineInterface
{
public:
	static GameState* CreateDefaults();
};