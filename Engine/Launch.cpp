#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NOCOMM
#include <Windows.h>
#include <iostream>
#include "GameLoop.h"

int main()
{
	GameLoop* Game = new GameLoop();
	int retur = Game->Start();
	delete Game;
	switch (retur)
	{
	case 10:
		printf("Failed to load INI file\n");
	default:
		break;
	}
	system("pause");
	return retur;
}