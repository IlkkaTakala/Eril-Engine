#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NOCOMM
#include <Windows.h>
#include "GameLoop.h"
#include "Interface/IRender.h"
#include <Interface/WindowManager.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	WindowManager::SetInstance(hInstance);
	Console::Init();
	GameLoop* Game = new GameLoop();
	if (InterfaceInit()) return 11;
	int retur = Game->Start();
	delete Game;
	switch (retur)
	{
	case 10:
		Console::Error("Failed to load INI file\n");
		break;
	case 11:
		Console::Error("Render error\n");
		break;
	default:
		break;
	}
	Console::Log("\nGame closed, press enter to continue\n");
	return retur;
}