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

	if (in->isText) {
		switch (key)
		{
		case 259:
		{
			if (action == GLFW_PRESS) in->Chars.push(key);
			return;
		}
		default:
			break;
		}
	}

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

}

void GLInput::ProcessInputs(float delta)
{
	WindowManager::PollEvents();

	/*for (auto const& [key, value] : Hold) {
		auto t = KeyCallersHold.find(key);
		if (t != KeyCallersHold.end()) {
			t->second(delta, true);
		}
	}*/
	while (!Inputs.empty()) {
		KeyAction key = Inputs.front();
		for (const auto& ic : ICs) {
			if(!ic->GetInputDisabled())
			ic->HandleInputs({key.key, key.scancode, key.action, key.mods }, delta);
		}
		Inputs.pop();
	}
	
	float x = 0.0, y = 0.0;
	//glfwGetCursorPos(dynamic_cast<Renderer*>(RI)->Window, &x, &y);
	//WindowManager::GetCursorPosition(dynamic_cast<Renderer*>(RI)->Window, x, y); // TODO
	for (const auto& m : ICs) {
		if (!m->GetInputDisabled())
		m->HandleMouse(x, y, delta);
	}
}

void GLInput::SetInputHandler(void(*Callback) (int, int, int, int))
{
	//KeyInput = Callback == nullptr ? &InputCallback : Callback;
	/*glfwSetKeyCallback((GLFWwindow*)WindowManager::GetHandle(dynamic_cast<Renderer*>(RI)->Window), [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		InputCallback(key, scancode, action, mods);
	});

	glfwSetMouseButtonCallback((GLFWwindow*)WindowManager::GetHandle(dynamic_cast<Renderer*>(RI)->Window), [](GLFWwindow* window, int key, int action, int mods) {
		MouseInputCallback(key, action, mods);
	});

	glfwSetCharCallback((GLFWwindow*)WindowManager::GetHandle(dynamic_cast<Renderer*>(RI)->Window), [](GLFWwindow* window, uint key) {
		TextInputCallback(key);
	});*/
	// TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
}
