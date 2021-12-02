#pragma once
#include "Core.h"
#include "Interface/IRender.h"
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
	friend void MouseInputCallback(int, int, int);
	friend void TextInputCallback(uint);

	static void (*KeyInput)(int, int, int, int);

	std::queue<KeyAction> Inputs;
	std::queue<uint> Chars;
	std::map<int, KeyAction> Hold;
};