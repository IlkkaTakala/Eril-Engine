#pragma once
#include <list>

class Shader;
class RenderBatch;
class GLCamera;
class RenderBuffer;

struct GLFWwindow;

class Renderer : public IRender
{
public:
	Renderer();
	virtual ~Renderer();

	virtual int SetupWindow() override;
	virtual void CleanRenderer() override;

	virtual Camera* CreateCamera(VisibleObject* parent = nullptr) override;
	virtual void SetActiveCamera(Camera*) override;

	virtual void LoadShaders() override;
	virtual Material* GetMaterialByName(String name) const override;
	virtual Material* LoadMaterialByName(String name) override;

	virtual void Update() override;
	virtual void Render() override;

private:
	friend class GLInput;

	std::map<String, Shader*> Shaders;
	std::map<String, Material*> BaseMaterials;

	GLFWwindow* Window;
	RenderBatch* Batcher;
	RenderBuffer* Buffer;
	GLCamera* ActiveCamera;
	uint ScreenPlane;
	uint ScreenVao;
	uint ScreenVbo;
	uint ScreenTexVbo;
};

class GLMesh : public IMesh
{
public:
	GLMesh();
	virtual ~GLMesh();
	virtual	RenderMesh* LoadData(VisibleObject* parent, String name) override;
	virtual void StartLoading() override;

private:
	std::map<std::string, std::ifstream*> ModelStreams;
	String ActiveDir;
};