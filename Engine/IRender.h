#pragma once
#include "Core.h"

#define OPENGL

class Material;
class RenderObject;

class IRender
{
public: 
	virtual int SetupWindow() = 0;
	virtual void CleanRenderer() = 0;

	virtual void Update() = 0;
	virtual void Render() = 0;
};

class IInput
{
public:
	virtual void SetInputHandler(void(*Callback)(int, int, int, int)) = 0;
};

class IMesh
{
public:
	virtual RenderObject* LoadData() = 0;
};

extern IRender* RI;
extern IInput* II;
extern IMesh* MI;

int InterfaceInit();