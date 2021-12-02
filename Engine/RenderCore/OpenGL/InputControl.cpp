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

void MouseInputCallback(int button, int action, int mods)
{
	GLInput* in = dynamic_cast<GLInput*>(II);

	KeyAction next;
	next.key = button;
	next.scancode = 0;
	next.action = action;
	next.mods = mods;

	in->Inputs.push(next);
}

void TextInputCallback(uint character)
{
	GLInput* in = dynamic_cast<GLInput*>(II);

	in->Chars.push(character);
}

GLInput::GLInput() : IInput()
{
	KeyInput = &InputCallback;
	isText = false;
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
		auto t = KeyCallersHold.find(key);
		if (t != KeyCallersHold.end()) {
			t->second(delta, true);
		}
	}
	while (!Inputs.empty()) {
		KeyAction key = Inputs.front();
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
		Inputs.pop();
	}
	while (!Chars.empty() && isText) {
		for (const auto& c : TextCallers) {
			c(Chars.front());
		}
		Chars.pop();
	}
	while (!Chars.empty()) Chars.pop();
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

	glfwSetMouseButtonCallback((GLFWwindow*)WindowManager::GetHandle(dynamic_cast<Renderer*>(RI)->Window), [](GLFWwindow* window, int key, int action, int mods) {
		MouseInputCallback(key, action, mods);
	});

	glfwSetCharCallback((GLFWwindow*)WindowManager::GetHandle(dynamic_cast<Renderer*>(RI)->Window), [](GLFWwindow* window, uint key) {
		TextInputCallback(key);
	});
}
