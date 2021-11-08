#pragma once

class WindowImplementation
{
public:

	static void StoreInstance(void* instance) { WindowInstance = instance; }

	static void* GetInstance() { return WindowInstance; }

private:
	static void* WindowInstance;
};

