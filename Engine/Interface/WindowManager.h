#pragma once
#include <BasicTypes.h>

typedef void (*CallbackKey)(int, int, int, int);

namespace WindowManager
{
	void AddWindow();

	void SetInstance(void* instance);
	void* GetInstance();

	uint CreateMainWindow(int width, int height);
	void UpdateWindow(uint window);
	void CloseWindow(uint window);
	void Terminate();
	void* GetHandle(uint window);

	Vector2D GetWindowSize(uint window);
	void SetWindowTitle(uint window, String title);

	float GetWindowTime();
	bool GetShouldWindowClose(uint window);

	void PollEvents();

	void GetCursorPosition(uint window, float& x, float& y);

	void SetShowCursor(uint window, bool show);
	bool GetShowCursor(uint window);
};

