#include "IRender.h"

#ifdef OPENGL
#include "OpenGLRenderer/Renderer.h"
#include "OpenGLRenderer/InputControl.h"
#endif // OPENGL


extern IRender* RI = nullptr;
extern IInput* II = nullptr;
extern IMesh* MI = nullptr;

int InterfaceInit()
{
	#ifdef OPENGL

	RI = new Renderer();
	II = new GLInput();
	MI = new GLMesh();

	#endif // OPENGL

	return 0;
}
