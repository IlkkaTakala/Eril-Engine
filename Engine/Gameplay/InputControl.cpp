#include <string>
#include "IRender.h"
#include "InputControl.h"

std::map<int, std::function<void(bool)>> Input::KeyCallers;
std::map<int, std::function<void(float, float)>> Input::MouseCallers;

void InputCallback(int key, int code, int action, int mods)
{
	if (!Input::KeyCallers.empty()) {
		auto t = Input::KeyCallers.find(key);
		if (t != Input::KeyCallers.end()) {
			if (action == 0) {
				t->second(false);
			}
			else if (action == 1) {
				t->second(true);
			}
		}
	}
}

Input::Input()
{
	II->SetInputHandler(&InputCallback);
}

Input::~Input()
{
	KeyCallers.clear();
	MouseCallers.clear();
}
