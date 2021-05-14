#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NOCOMM
#include <Windows.h>
#include <iostream>
#include "GameLoop.h"
#include "IRender.h"

int main()
{
	GameLoop* Game = new GameLoop();
	if (InterfaceInit()) return 11;
	int retur = Game->Start();
	delete Game;
	switch (retur)
	{
	case 10:
		printf("Failed to load INI file\n");
		break;
	case 11:
		printf("Render error\n");
		break;
	default:
		break;
	}
	return retur;
}