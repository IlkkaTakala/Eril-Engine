#pragma once

class WindowImplementation;

class WindowManager
{
public:
	static void AddWindow();

	static void SetInstance(void* instance);
	static void* GetInstance();

private:
	static WindowImplementation* Impl;
};

