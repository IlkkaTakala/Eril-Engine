#include <string>
#include "InputControl.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "Renderer.h"
#include <Interface/WindowManager.h>

void (*GLInput::KeyInput)(int, int, int, int) = nullptr;

void InputCallback(int key, int code, int action, int mods)
{
	GLInput* in = dynamic_cast<GLInput*>(II);

	KeyAction next;
	next.key = key;
	next.scancode = code;
	next.action = action;
	next.mods = mods;

	in->Inputs.push(next);
}

GLInput::GLInput() : IInput()
{
	KeyInput = &InputCallback;
}

GLInput::~GLInput()
{
	KeyCallers.clear();
	MouseCallers.clear();
}

void GLInput::ProcessInputs(float delta)
{
	WindowManager::PollEvents();

	for (auto const& [key, value] : Hold) {
		auto t = KeyCallers.find(key);
		if (t != KeyCallers.end()) {
			t->second(delta, true);
		}
	}
	while (!Inputs.empty()) {
		KeyAction key = Inputs.front();
		switch (key.action)
		{
		case GLFW_PRESS:
			if (Hold.find(key.key) == Hold.end()) {
				auto t = KeyCallers.find(key.key);
				if (t != KeyCallers.end()) {
					t->second(delta, true);
				}
				Hold[key.key] = key;
			}
			break;
		case GLFW_RELEASE:
			if (Hold.find(key.key) != Hold.end()) {
				auto t = KeyCallers.find(key.key);
				if (t != KeyCallers.end()) {
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
		Inputs.pop();
	}
	float x = 0.0, y = 0.0;
	//glfwGetCursorPos(dynamic_cast<Renderer*>(RI)->Window, &x, &y);
	WindowManager::GetCursorPosition(dynamic_cast<Renderer*>(RI)->Window, x, y);
	auto t = MouseCallers.equal_range(0);
	for (auto it = t.first; it != t.second; it++) {
		it->second(MouseX - x, MouseY - y);
	}
	MouseX = x;
	MouseY = y;
}

void GLInput::SetInputHandler(void(*Callback) (int, int, int, int))
{
	//KeyInput = Callback == nullptr ? &InputCallback : Callback;
	glfwSetKeyCallback((GLFWwindow*)WindowManager::GetHandle(dynamic_cast<Renderer*>(RI)->Window), [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		InputCallback(key, scancode, action, mods);
	});
}
