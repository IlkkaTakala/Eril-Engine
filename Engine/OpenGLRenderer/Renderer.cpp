#include "Material.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "Core.h"
#include "Renderer.h"

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

int Renderer::SetupWindow()
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

void Renderer::CleanRenderer()
{
	glfwDestroyWindow(Window);
	glfwTerminate();
}

void Renderer::Update()
{
	glfwPollEvents();
	if (glfwWindowShouldClose(Window)) Exit();
}

void Renderer::Render()
{

	for (Shader* s : Shaders)
	{
		s->Bind();

		for (Material* m : s->GetUsers())
		{
			//s->SetUniform();

			for (Section* o : m->GetObjects())
			{

				glDrawArrays(GL_TRIANGLES, 0, 6);
			}
		}
	}

	glfwSwapBuffers(Window);
}

void (*KeyInput)(int, int, int, int) = nullptr;

void GLInput::SetInputHandler(void(*Callback) (int, int, int, int))
{
	KeyInput = Callback;
	glfwSetKeyCallback(reinterpret_cast<Renderer*>(RI)->Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		KeyInput(key, scancode, action, mods);
	});
}

RenderObject* GLMesh::LoadData()
{
	return nullptr;
}
