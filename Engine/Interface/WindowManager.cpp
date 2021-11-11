#include "WindowManager.h"

static void* WindowInstance = nullptr;

void WindowManager::AddWindow()
{
}

void WindowManager::SetInstance(void* instance)
{
	WindowInstance = instance;
}

void* WindowManager::GetInstance()
{
	return WindowInstance;
}
