#include "InputComponent.h"
#include <string>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "Renderer.h"
#include <Interface/WindowManager.h>

InputComponent::InputComponent()
{
	II->RegisterInputComponent(this);
	MouseX = 0;
	MouseY = 0;
	isText = false;
}

void InputComponent::OnDestroyed()
{
	II->UnregisterInputComponent(this);
}

void InputComponent::Tick(float delta)
{
	for (auto const& [key, value] : Hold) {
		auto t = KeyCallersHold.find(key);
		if (t != KeyCallersHold.end()) {
			t->second(delta, true);
		}
	}
}

void InputComponent::HandleInputs(KeyActioni key, float delta)
{
	if (!isText || (isText && (key.key >= 96 || key.key <= 32 || key.key <= 6))) {
		switch (key.action)
		{
		case GLFW_PRESS:
			if (KeyCallers.find(key.key) != KeyCallers.end()) {
				auto it = KeyCallers.equal_range(key.key);
				while (it.first != it.second) {
					it.first->second(true);
					it.first++;
				}
			}
			else if (Hold.find(key.key) == Hold.end()) {
				auto t = KeyCallersHold.find(key.key);
				if (t != KeyCallersHold.end()) {
					t->second(delta, true);
				}
				Hold[key.key] = key;
			}
			break;
		case GLFW_RELEASE:
			if (KeyCallers.find(key.key) != KeyCallers.end()) {
				auto it = KeyCallers.equal_range(key.key);
				while (it.first != it.second) {
					it.first->second(false);
					it.first++;
				}
			}
			if (Hold.find(key.key) != Hold.end()) {
				auto t = KeyCallersHold.find(key.key);
				if (t != KeyCallersHold.end()) {
					t->second(delta, false);
				}
				Hold.erase(key.key);
			}
			break;
		case GLFW_REPEAT:

			break;
		default:
			break;
		}
	}
}

void InputComponent::HandleMouse(float x, float y, float delta)
{
	auto t = MouseCallers.equal_range(0);
	for (auto it = t.first; it != t.second; it++) {
		it->second(MouseX - x, MouseY - y);
	}
	MouseX = x;
	MouseY = y;
}
