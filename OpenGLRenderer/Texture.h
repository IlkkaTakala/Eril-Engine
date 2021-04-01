#pragma once

#ifdef OPENGLRENDERER_EXPORTS
#define OPENGL __declspec(dllexport)
#else
#define OPENGL __declspec(dllimport)
#endif

class OPENGL Texture;