#pragma once
#include <list>

class Shader;

struct GLFWwindow;

class Renderer : public IRender
{
public:
	Renderer();
	virtual ~Renderer();

	virtual int SetupWindow() override;
	virtual void CleanRenderer() override;

	virtual void Update() override;
	virtual void Render() override;

private:
	friend class GLInput;

	std::list<Shader*> Shaders;

	GLFWwindow* Window = nullptr;
};

class GLInput : public IInput
{
	virtual void SetInputHandler(void(*Callback)(int, int, int, int)) override;

protected:
};

class GLMesh : public IMesh
{
public:
	virtual	RenderObject* LoadData();
};