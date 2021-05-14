#pragma once
#include "Core.h"
#include "IRender.h"
#include <map>
#include <queue>

struct KeyAction
{
	int key;
	int scancode;
	int action;
	int mods;
};

class GLInput : public IInput
{
public:
	GLInput();
	virtual ~GLInput();

	virtual void ProcessInputs(float delta) override;
	virtual void SetInputHandler(void(*Callback)(int, int, int, int) = 0) override;

private:
	friend void InputCallback(int, int, int, int);

	static void (*KeyInput)(int, int, int, int);

	std::queue<KeyAction> Inputs;
	std::map<int, KeyAction> Hold;
};