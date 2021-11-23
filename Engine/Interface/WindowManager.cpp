#include "WindowManager.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <Core.h>
#include <Settings.h>

static void* WindowInstance = nullptr;
static std::map<uint, GLFWwindow*> Windows;

void WindowManager::AddWindow()
{
}

void WindowManager::SetInstance(void* instance)
{
	WindowInstance = instance;
}

void* WindowManager::GetInstance()
{
	return WindowInstance;
}

uint WindowManager::CreateMainWindow(int width, int height)
{
	if (!glfwInit()) {
		return -1;
	}
	//fpsCounter = INI->GetValue("Engine", "ShowFps") == "true";

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	Console::Log("Creating window...");
	GLFWwindow* Window = glfwCreateWindow(width, height, "Eril Engine Demo | Loading...", NULL, NULL);
	if (!Window) {
		glfwTerminate();
		return -1;
	}
	uint idx = Windows.size() > 0 ? Windows.rbegin()->first + 1 : 1;
	Windows.emplace(idx, Window);

	if (INI->GetValue("Render", "ResolutionY").c_str() == "false")
		glfwSetWindowAttrib(Window, GLFW_DECORATED, GLFW_FALSE);
	glfwSetInputMode(Window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
	
	SetShowCursor(idx, false);

	glfwMakeContextCurrent(Window);
	if (INI->GetValue("Render", "VSync") == "false") glfwSwapInterval(0);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		Console::Log("Failed to initialize OpenGL context");
		return -1;
	}
	
	return idx;
}

void WindowManager::UpdateWindow(uint window)
{
	glfwSwapBuffers(Windows[window]);
}

void WindowManager::CloseWindow(uint window)
{
	glfwDestroyWindow(Windows[window]);
}

void WindowManager::Terminate()
{
	glfwTerminate();
}

void* WindowManager::GetHandle(uint window)
{
	return Windows[window];
}

Vector2D WindowManager::GetWindowSize(uint window)
{
	int width;
	int height;
	glfwGetFramebufferSize(Windows[window], &width, &height);
	return Vector2D(width, height);
}

void WindowManager::SetWindowTitle(uint window, String title)
{
	glfwSetWindowTitle(Windows[window], title.c_str());
}

float WindowManager::GetWindowTime()
{
	return (float)glfwGetTime();
}

bool WindowManager::GetShouldWindowClose(uint window)
{
	return glfwWindowShouldClose(Windows[window]);
}

void WindowManager::PollEvents()
{
	glfwPollEvents();
}

void WindowManager::GetCursorPosition(uint window, float& x, float& y)
{
	double dx;
	double dy;
	glfwGetCursorPos(Windows[window], &dx, &dy);
	x = (float)dx;
	y = (float)dy;
}

void WindowManager::SetShowCursor(uint window, bool show)
{
	if (show) {
		glfwSetInputMode(Windows[window], GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		if (glfwRawMouseMotionSupported())
			glfwSetInputMode(Windows[window], GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
	}
	else {
		glfwSetInputMode(Windows[window], GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		if (glfwRawMouseMotionSupported())
			glfwSetInputMode(Windows[window], GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}
}

bool WindowManager::GetShowCursor(uint window)
{
	int mode = glfwGetInputMode(Windows[window], GLFW_CURSOR);
	return mode == GLFW_CURSOR_NORMAL;
}
