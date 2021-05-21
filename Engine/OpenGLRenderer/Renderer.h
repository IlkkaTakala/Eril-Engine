#pragma once
#include <list>

class Shader;
class RenderBatch;
class GLCamera;
class RenderBuffer;
struct LightData;

struct GLFWwindow;

struct Globals;

class Renderer : public IRender
{
public:
	Renderer();
	virtual ~Renderer();

	virtual int SetupWindow(int width, int height) override;
	virtual void CleanRenderer() override;

	virtual Camera* CreateCamera(VisibleObject* parent = nullptr) override;
	virtual void SetActiveCamera(Camera*) override;
	virtual void CreateLight(const LightData*) override;
	virtual void RemoveLight(const LightData*) override;
	void UpdateLights();

	virtual void LoadShaders() override;
	virtual Material* GetMaterialByName(String name) const override;
	virtual Material* LoadMaterialByName(String name) override;

	virtual void Update() override;
	virtual void Render() override;

private:
	friend class GLInput;

	void Deferred(int width, int height);
	void Forward(int width, int height);
	void LightCulling(int width, int height);
	void DeferredLighting(int width, int height);

	std::map<String, Shader*> Shaders;
	std::map<String, Material*> BaseMaterials;
	std::vector<const LightData*> Lights;

	Shader* LightCullingShader;
	Shader* DeferredMaster;
	GLFWwindow* Window;
	RenderBatch* Batcher;
	RenderBuffer* Buffer;
	GLCamera* ActiveCamera;

	uint ScreenVao;
	uint ScreenVbo;
	uint ScreenTexVbo;

	uint LightBuffer;
	uint VisibleLightIndicesBuffer;
	uint GlobalUniforms;

	uint MaxLightCount;
	uint WorkGroupsX;
	uint WorkGroupsY;
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