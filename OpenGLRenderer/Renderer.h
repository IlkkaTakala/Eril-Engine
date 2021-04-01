#pragma once

#ifdef OPENGLRENDERER_EXPORTS
#define OPENGL __declspec(dllexport)
#else
#define OPENGL __declspec(dllimport)
#endif

class OPENGL RenderData;

class OPENGL RI
{
public:
	static int SetupWindow();
	static void CleanRenderer();

	static void Update();
	static void Render();
private:
};

namespace InputInterface
{
	OPENGL void SetInputHandler(void(*Callback)(int, int, int, int));
}