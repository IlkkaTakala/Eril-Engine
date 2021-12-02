#pragma once
#include <list>

class Shader;
class Texture;
class GLCamera;
class PreDepthBuffer;
class PostBuffer;
class BlurBuffer;
class SSAOBuffer;
class ShadowMapBuffer;
class ReflectionBuffer;
struct LightData;
class UISpace;

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
	/*
	virtual void CreateLight(const LightData*) override;
	virtual void RemoveLight(const LightData*) override;
	*/
	void UpdateLights();

	virtual void LoadShaders() override;
	virtual Material* GetMaterialByName(String name) const override;
	virtual Material* LoadMaterialByName(String name) override;
	virtual Texture* LoadTextureByName(String name) override;

	virtual void Update() override;
	virtual void Render(float delta) override;
	virtual void GameStart() override;
	virtual void DestroyWindow() override;

	virtual UISpace* GetUIManager(int screen = 0) const override { return UIHolder; }

private:
	friend class GLInput;

	void EnvReflection(int width, int height);
	void EnvCube(int width, int height);
	void Shadows(int width, int height);
	void SSAO(int width, int height);
	void Forward(int width, int height);
	void PreDepth(int width, int height);
	void LightCulling(int width, int height);
	void UpdateTransforms();

	std::map<String, Shader*> Shaders;
	std::map<String, Material*> BaseMaterials;
	std::map<String, Texture*> LoadedTextures;
	//std::vector<const LightData*> Lights;

	Shader* LightCullingShader;
	Shader* PreDepthShader;
	Shader* PostProcessMaster;
	Shader* CompositeShader;
	Shader* SSAOShader;
	Shader* SSAOBlurShader;
	Shader* ShadowShader;
	Shader* ShadowColorShader;
	Shader* SkyDomeShader;
	Shader* SkyFilterShader;
	Shader* SkyBoxShader;
	uint Window;
	PreDepthBuffer* DepthBuffer;
	PostBuffer* PostProcess;
	BlurBuffer* BlurRender;
	SSAOBuffer* SSAORender;
	ShadowMapBuffer* ShadowMapping;
	ReflectionBuffer* EnvironmentRender;
	GLCamera* ActiveCamera;
	UISpace* UIHolder;

	uint ScreenVao;
	uint ScreenVbo;
	uint ScreenTexVbo;

	uint LightBuffer;
	uint VisibleLightIndicesBuffer;
	uint GlobalUniforms;
	uint SSAOKernelBuffer;
	uint SSAONoise;

	uint EnvironmentVAO;
	uint EnvironmentVBO;

	uint EnvSizeX;
	uint EnvSizeY;

	uint MaxLightCount;
	uint WorkGroupsX;
	uint WorkGroupsY;

	bool fpsCounter;
};

class GLMesh : public IMesh
{
public:
	GLMesh();
	virtual ~GLMesh();
	virtual	RenderMesh* LoadData(VisibleObject* parent, String name) override;
	virtual RenderMesh* CreateProcedural(VisibleObject* parent, String name, std::vector<Vector>& positions, std::vector<Vector> UV, std::vector<Vector>& normal, std::vector<Vector>& tangent, std::vector<uint32>& indices) override;
	virtual void StartLoading() override;

private:
	//std::map<std::string, std::ifstream*> ModelStreams;
	std::map<String, String> ModelStreams;
	String ActiveDir;
};