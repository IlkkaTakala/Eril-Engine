#include "pch.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>	
#include "Renderer.h"

GLFWwindow* Window = nullptr;

namespace InputInterface
{
	void (*KeyInput)(int, int, int, int);
}

int RI::SetupWindow()
{
	//glfwSetErrorCallback([](int error, const char* description) {
	//	fprintf(stderr, "Error %d: %s\n", error, description);
	//});

	if (!glfwInit()) {
		return -1;
	}

	Window = glfwCreateWindow(640, 480, "OpenGL window", NULL, NULL);
	if (!Window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(Window);
	gladLoadGL(glfwGetProcAddress);

	return 0;
}

void RI::CleanRenderer()
{
	glfwDestroyWindow(Window);
	glfwTerminate();
}


void RI::Update()
{
	glfwPollEvents();
}

void RI::Render()
{
	glfwSwapBuffers(Window);
}

void InputInterface::SetInputHandler(void(*Callback) (int, int, int, int))
{
	KeyInput = Callback;
	glfwSetKeyCallback(Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		// Close window if escape is pressed by the user.
		InputInterface::KeyInput(key, scancode, action, mods);
	});
}