#pragma once
#include <list>
#include <RenderCore/RenderCommands.h>
#include <Basic/QueueSafe.h>

class Shader;
class Texture;
class GLCamera;
class PreDepthBuffer;
class PostBuffer;
class BlurBuffer;
class SSAOBuffer;
class ShadowMapBuffer;
class ReflectionBuffer;
//struct LightData; //Lights have been moved to be handled by the ECS-system.
class LightComponent;
class UISpace;
class Section;

struct GLFWwindow;

struct Globals;

class Renderer
{
public:
	Renderer();
	virtual ~Renderer();

	int SetupWindow(int width, int height);
	void CleanRenderer();

	Camera* CreateCamera(SceneComponent* parent = nullptr);
	void SetActiveCamera(Camera*);
	Camera* GetActiveCamera() const;
	
	void UpdateLights();

	void LoadShaders();
	Material* GetMaterialByName(const String& name) const;
	Material* LoadMaterialByName(const String& name);

	static void Update(SafeQueue<RenderCommand>* commands, Renderer* RC);
	void Render(float delta);
	void GameStart();
	void DestroyWindow();

	virtual UISpace* GetUIManager(int screen = 0) const { return UIHolder; }

	virtual uint GetMainWindowHandle() const;

private:
	friend class GLInput;
	friend class GLCamera;

	void EnvReflection(int width, int height);
	void EnvCube(int width, int height);
	void Shadows(int width, int height);
	void SSAO(int width, int height);
	void Forward(int width, int height);
	void PreDepth(int width, int height);
	void LightCulling(int width, int height);
	void UpdateTransforms();
	inline bool CullCheck(Section* s);

	std::map<String, Shader*> Shaders;
	std::map<String, Material*> BaseMaterials;
	//std::vector<const LightData*> Lights;

	bool ready;

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

	std::vector<LightComponent>* Lights;
};