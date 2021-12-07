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
	Console::Log("Hello, Eril is starting");
	Console::Error("This world is an error");
	GameLoop* Game = new GameLoop();
	if (InterfaceInit()) return 11;
	int retur = Game->Start();
	delete Game;
	switch (retur)
	{
	case 10:
		Console::Error("Failed to load INI file");
		break;
	case 11:
		Console::Error("Render error");
		break;
	default:
		break;
	}
	Console::Log("Game closed, goodbye");
	Sleep(10000);
	Console::Close();
	return retur;
}