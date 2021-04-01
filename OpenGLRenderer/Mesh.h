#pragma once

#ifdef OPENGLRENDERER_EXPORTS
#define OPENGL __declspec(dllexport)
#else
#define OPENGL __declspec(dllimport)
#endif

class OPENGL MeshData;

namespace IMesh 
{
	static MeshData* LoadData();
}

