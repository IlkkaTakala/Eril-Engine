#include "WindowManager.h"
#include "WindowImplementation.h"

void WindowManager::AddWindow()
{
}

void WindowManager::SetInstance(void* instance)
{
	WindowImplementation::StoreInstance(instance);
}

void* WindowManager::GetInstance()
{
	return WindowImplementation::GetInstance();
}
