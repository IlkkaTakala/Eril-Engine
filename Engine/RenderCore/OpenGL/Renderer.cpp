#include "Material.h"
#include <glad/gl.h>
#include "Core.h"
#include <Interface/WindowManager.h>
#include "Renderer.h"
#include "Camera.h"
#include "Mesh.h"
#include "Texture.h"
#include "RenderBuffer.h"
#include <Objects/VisibleObject.h>
#include "LightData.h"
#include "Settings.h"
#include <filesystem>
#include <stdexcept>
#include <iostream>
#include <random>
#include "UI/UISpace.h"
#include "UI/Panel.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

struct Globals
{
	glm::mat4 Projection;
	glm::mat4 View;
	glm::vec4 ViewPoint;
	glm::ivec2 ScreenSize;
	int SceneLightCount;
};

Globals GlobalVariables;
glm::mat4 ShadowProj;
glm::mat4 ShadowOrtho;

struct GLM_Light
{
	glm::vec4 color;
	glm::vec4 locationAndSize;
	glm::vec4 rotation;
	glm::ivec4 type;

	GLM_Light() {
		color = glm::vec4(0.0);
		locationAndSize = glm::vec4(0.0);
		rotation = glm::vec4(0.0);
		type = glm::ivec4(1);
	}
};

Renderer::Renderer()
{
	DepthBuffer = nullptr;
	BlurRender = nullptr;
	Window = 0;
	ActiveCamera = nullptr;
	PreDepthShader = nullptr;
	LightCullingShader = nullptr;
	SSAOShader = nullptr;
	SSAOBlurShader = nullptr;
	SSAORender = nullptr;
	PostProcess = nullptr;
	PostProcessMaster = nullptr;
	CompositeShader = nullptr;
	ShadowShader = nullptr;
	ShadowMapping = nullptr;
	ShadowColorShader = nullptr;
	EnvironmentRender = nullptr;
	SkyDomeShader = nullptr;
	SkyBoxShader = nullptr;
	UIHolder = nullptr;
	SkyFilterShader = nullptr;

	MaxLightCount = 1024;

	EnvironmentVAO = 0;
	EnvironmentVBO = 0;

	ScreenVao = 0;
	ScreenVbo = 0;
	ScreenTexVbo = 0;

	EnvSizeX = 1024;
	EnvSizeY = 1024;

	GlobalUniforms = 0;
	LightBuffer = 0;
	SSAOKernelBuffer = 0;
	SSAONoise = 0;
	VisibleLightIndicesBuffer = 0;
	WorkGroupsX = 0;
	WorkGroupsY = 0;
	fpsCounter = 0;

}

Renderer::~Renderer()
{
	for (auto const& m : BaseMaterials) {
		delete m.second;
	}

	for (auto const& s : Shaders) {
		delete s.second;
	}

	for (auto const& t : LoadedTextures) {
		delete t.second;
	}

	delete DepthBuffer;
	delete PostProcess;
	delete BlurRender;
	delete SSAORender;
	delete EnvironmentRender;
	delete PreDepthShader;
	delete PostProcessMaster;
	delete CompositeShader;
	delete LightCullingShader;
	delete SSAOShader;
	delete ShadowShader;
	delete ShadowMapping;
	delete SkyDomeShader;
	delete SkyBoxShader;
	delete UIHolder;
}

inline float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;
	char hex_string1[20];
	char hex_string2[20];
	sprintf(hex_string1, "%X", type);
	sprintf(hex_string2, "%X", severity);
	if (severity == GL_DEBUG_SEVERITY_LOW) Console::Warning(String("OpenGL Error: type = ") + hex_string1 + ", severity = " + hex_string2 + ", message = " + message);
	else Console::Error(String("OpenGL Error: type = ") + hex_string1 + ", severity = " + hex_string2 + ", message = " + message);
}

int Renderer::SetupWindow(int width, int height)
{
	if (width < 640 || width > 2048 || height < 480 || height > 2048) throw std::exception("Unsupported resolution!\n");

	Window = WindowManager::CreateMainWindow(width, height);

	// During init, enable debug output
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	Console::Log("Allocating buffers...");
	DepthBuffer = new PreDepthBuffer(width, height);
	PostProcess = new PostBuffer(width, height);
	SSAORender = new SSAOBuffer(width, height);
	ShadowMapping = new ShadowMapBuffer(width, height);
	EnvironmentRender = new ReflectionBuffer(EnvSizeX, EnvSizeY);

	float vertices[] = {
		-1.f, -1.f, 1.f,
		1.f, -1.f, 1.f,
		1.f, 1.f, 1.f,
		-1.f, -1.f, 1.f,
		1.f, 1.f, 1.f,
		-1.f, 1.f, 1.f
	};

	float texCoords[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};

	glGenVertexArrays(1, &ScreenVao);
	glGenBuffers(1, &ScreenVbo);
	glGenBuffers(1, &ScreenTexVbo);

	glBindVertexArray(ScreenVao);

	glBindBuffer(GL_ARRAY_BUFFER, ScreenVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, ScreenTexVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	WorkGroupsX = (width + (width % 16)) / 16;
	WorkGroupsY = (height + (height % 16)) / 16;
	size_t numberOfTiles = WorkGroupsX * WorkGroupsY;

	glGenBuffers(1, &LightBuffer);
	glGenBuffers(1, &VisibleLightIndicesBuffer);

	// Bind light buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, MaxLightCount * sizeof(GLM_Light), 0, GL_DYNAMIC_DRAW);

	// Bind visible light indices buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, VisibleLightIndicesBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, numberOfTiles * sizeof(int) * 1024, 0, GL_STATIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glGenBuffers(1, &GlobalUniforms);
	glBindBuffer(GL_UNIFORM_BUFFER, GlobalUniforms);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Globals), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);

	glViewport(0, 0, width, height);
	glClearColor(0.f, 0.f, 0.f, 0.f);

	Console::Log("Loading shaders...");
	LoadShaders();

	if (LightCullingShader == nullptr || PreDepthShader == nullptr) throw std::exception("Important shaders not found!\n");

	Shader* blur = Shaders.find("BlurCompute") == Shaders.end() ? nullptr : Shaders.find("BlurCompute")->second;
	BlurRender = new BlurBuffer(width, height, blur);

	if (SSAOShader != nullptr && SSAOBlurShader != nullptr) {
		std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between [0.0, 1.0]
		std::default_random_engine generator;
		std::vector<glm::vec3> ssaoKernel;
		for (unsigned int i = 0; i < 64; ++i)
		{
			glm::vec3 sample(
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator)
			);
			sample = glm::normalize(sample);
			sample *= randomFloats(generator);
			float scale = (float)i / 64.f;
			scale = lerp(0.1f, 1.0f, scale * scale);
			sample *= scale;
			ssaoKernel.push_back(sample);
		}

		glGenBuffers(1, &SSAOKernelBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSAOKernelBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, ssaoKernel.size() * sizeof(glm::vec3), &ssaoKernel[0], GL_STATIC_DRAW);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		std::vector<glm::vec3> ssaoNoise;
		for (unsigned int i = 0; i < 16; i++)
		{
			glm::vec3 noise(
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator) * 2.0 - 1.0,
				0.0f);
			ssaoNoise.push_back(noise);
		}

		glGenTextures(1, &SSAONoise);
		glBindTexture(GL_TEXTURE_2D, SSAONoise);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, ssaoNoise.data());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

	float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
	float near = 0.1f;
	float far = 1000.0f;
	ShadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far);
	ShadowOrtho = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near, far);

	float envmap[] = {
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	glGenVertexArrays(1, &EnvironmentVAO);
	glGenBuffers(1, &EnvironmentVBO);

	glBindVertexArray(EnvironmentVAO);

	// Set buffer data to m_vbo-object (bind buffer first and then set the data)
	glBindBuffer(GL_ARRAY_BUFFER, EnvironmentVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(envmap), envmap, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	UIHolder = new UISpace();
	UIHolder->SetSize(width, height);
	UIHolder->SetScreen(Window);
	return 0;
}

void Renderer::CleanRenderer()
{
	WindowManager::CloseWindow(Window);

	glDeleteBuffers(1, &LightBuffer);
	glDeleteBuffers(1, &VisibleLightIndicesBuffer);
	glDeleteBuffers(1, &GlobalUniforms);
	glDeleteBuffers(1, &SSAOKernelBuffer);
	glDeleteTextures(1, &SSAONoise);

	glDeleteBuffers(1, &ScreenVbo);
	glDeleteBuffers(1, &ScreenTexVbo);
	glDeleteBuffers(1, &EnvironmentVBO);
	glDeleteVertexArrays(1, &ScreenVao);
	glDeleteVertexArrays(1, &EnvironmentVAO);

	WindowManager::Terminate();
}

Camera* Renderer::CreateCamera(VisibleObject* parent)
{
	GLCamera* cam = new GLCamera(parent == nullptr ? nullptr : dynamic_cast<RenderObject*>(parent->GetModel()));
	return cam;
}

void Renderer::SetActiveCamera(Camera* cam)
{
	ActiveCamera = dynamic_cast<GLCamera*>(cam);
}

void Renderer::CreateLight(const LightData* light)
{
	Lights.push_back(light);
	LightCullingShader->SetUniform("lightCount", (int)Lights.size());
}	

void Renderer::RemoveLight(const LightData* light)
{
	for (auto i = Lights.begin(); i != Lights.end(); i++) {
		if (*i == light)
		{
			Lights.erase(i);
			break;
		}
	}
	LightCullingShader->SetUniform("lightCount", (int)Lights.size());
}

void Renderer::UpdateLights()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightBuffer);
	GLM_Light* mapped = reinterpret_cast<GLM_Light*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, MaxLightCount * sizeof(GLM_Light), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
	for (int i = 0; i < Lights.size(); i++) {
		GLM_Light light;
		light.locationAndSize = glm::vec4(Lights[i]->Location.X, Lights[i]->Location.Z, Lights[i]->Location.Y, Lights[i]->Size);
		glm::mat4 rot = glm::mat4(1.0) * glm::toMat4(glm::quat(glm::vec3(glm::radians(Lights[i]->Rotation.X), glm::radians(Lights[i]->Rotation.Y), glm::radians(Lights[i]->Rotation.Z))));
		light.rotation = rot * glm::vec4(0.0, -1.0, 0.0, 0.0);
		light.color = glm::vec4(Lights[i]->Color.X, Lights[i]->Color.Y, Lights[i]->Color.Z, 1.0) * Lights[i]->Intensity;
		light.type.x = Lights[i]->Type;
		mapped[i] = light;
	}
	int result = glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	if (result == GL_FALSE) exit(99);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

bool LoadVertex(std::ifstream& s, String& vertex)
{
	s.clear();
	s.seekg(0);
	String in_line;
	bool found = false;
	while (std::getline(s, in_line)) {
		if (in_line == "###VERTEX###") {
			found = true;
			break;
		}
	}
	if (found) {
		while (std::getline(s, in_line))
		{
			if (in_line != "###END_VERTEX###") {
				vertex += in_line + '\n';
			}
			else break;
		}
		return true;
	}
	return false;
}

bool LoadGeometry(std::ifstream& s, String& geom)
{
	s.clear();
	s.seekg(0);
	String in_line;
	bool found = false;
	while (std::getline(s, in_line)) {
		if (in_line == "###GEOMETRY###") {
			found = true;
			break;
		}
	}
	if (found) {
		while (std::getline(s, in_line))
		{
			if (in_line != "###END_GEOMETRY###") {
				geom += in_line + '\n';
			}
			else break;
		}
		return true;
	}
	return false;
}

bool LoadFragment(std::ifstream& s, String& fragment)
{
	s.clear();
	s.seekg(0);
	String in_line;
	bool found = false;
	while (std::getline(s, in_line)) {
		if (in_line == "###FRAGMENT###") {
			found = true;
			break;
		}
	}
	if (found) {
		while (std::getline(s, in_line))
		{
			if (in_line != "###END_FRAGMENT###") {
				fragment += in_line + '\n';
			}
			else break;
		}
		return true;
	}
	return false;
}

bool LoadCompute(std::ifstream& s, String& compute)
{
	s.clear();
	s.seekg(0);
	String in_line;
	bool found = false;
	while (std::getline(s, in_line)) {
		if (in_line == "###COMPUTE###") {
			found = true;
			break;
		}
	}
	if (found) {
		while (std::getline(s, in_line))
		{
			if (in_line != "###END_COMPUTE###") {
				compute += in_line + '\n';
			}
			else break;
		}
		return true;
	}
	return false;
}

void Renderer::LoadShaders()
{
	namespace fs = std::filesystem;
	String folder = INI->GetValue("Engine", "DataFolder") + "/Shaders";
	for (const auto& f : fs::recursive_directory_iterator(folder)) {
		if (f.path().extension() == ".shader") {
			std::ifstream stre = std::ifstream(f.path(), std::ios_base::in);

			
			String in_line;

			std::getline(stre, in_line);

			std::vector<String> params = split(in_line, ';');
			if (params.size() < 1) throw std::exception("Invalid Shader");

			switch (std::atoi(params[0].c_str()))
			{
			case 0:
			{
				String vertex;
				if (LoadVertex(stre, vertex)) {
					Shader* nShader = new Shader(1, vertex.c_str());
					nShader->Pass = std::atoi(params[1].c_str());
					if (params.size() > 2) nShader->FaceCulling = std::atoi(params[2].c_str());
					if (!nShader->Success) nShader = nullptr;
					if (nShader != nullptr) {
						if (f.path().filename() == "LightCullingShader.shader") LightCullingShader = nShader;
						else Shaders.emplace(f.path().filename().replace_extension("").string(), nShader);
					}
					else throw std::exception(("Invalid shaders found! : " + f.path().string() + '\n').c_str());
				}
			} break;

			case 1: 
			{
				String fragment;
				if (LoadFragment(stre, fragment)) {
					Shader* nShader = new Shader(2, fragment.c_str());
					nShader->Pass = std::atoi(params[1].c_str());
					if (params.size() > 2) nShader->FaceCulling = std::atoi(params[2].c_str());
					if (!nShader->Success) nShader = nullptr;
					if (nShader != nullptr) {
						Shaders.emplace(f.path().filename().replace_extension("").string(), nShader);
					}
					else throw std::exception(("Invalid shaders found! : " + f.path().string() + '\n').c_str());
				}
			} break;

			case 2:
			{
				String vertex;
				String fragment;
				if (LoadVertex(stre, vertex) && LoadFragment(stre, fragment)) {
					Shader* nShader = new Shader(vertex.c_str(), fragment.c_str());
					nShader->Pass = std::atoi(params[1].c_str());
					if (params.size() > 2) nShader->FaceCulling = std::atoi(params[2].c_str());
					if (!nShader->Success) nShader = nullptr;
					if (nShader != nullptr) {
						if (f.path().filename() == "PreDepth.shader") PreDepthShader = nShader;
						else if (f.path().filename() == "PostProcessMaster.shader") PostProcessMaster = nShader;
						else if (f.path().filename() == "Composite.shader") CompositeShader = nShader;
						else if (f.path().filename() == "SSAO.shader") SSAOShader = nShader;
						else if (f.path().filename() == "SSAOBlur.shader") SSAOBlurShader = nShader;
						else if (f.path().filename() == "ShadowColor.shader") ShadowColorShader = nShader;
						else if (f.path().filename() == "SkyBox.shader") SkyBoxShader = nShader;
						else Shaders.emplace(f.path().filename().replace_extension("").string(), nShader);
					}
					else {
						throw std::exception(("Invalid shaders found! : " + f.path().string() + '\n').c_str());
						delete nShader;
					}
				}
			}
			break;

			case 3:
			{
				String vertex;
				String fragment;
				String geom;
				if (LoadVertex(stre, vertex) && LoadFragment(stre, fragment) && LoadGeometry(stre, geom)) {
					Shader* nShader = new Shader(vertex.c_str(), geom.c_str(), fragment.c_str());
					nShader->Pass = std::atoi(params[1].c_str());
					if (params.size() > 2) nShader->FaceCulling = std::atoi(params[2].c_str());
					if (!nShader->Success) nShader = nullptr;
					if (nShader != nullptr) {
						if (f.path().filename() == "Shadow.shader") ShadowShader = nShader;
						else if (f.path().filename() == "SkyDome.shader") SkyDomeShader = nShader;
						else if (f.path().filename() == "SkyFilter.shader") SkyFilterShader = nShader;
						else Shaders.emplace(f.path().filename().replace_extension("").string(), nShader);
					}
					else throw std::exception(("Invalid shaders found! : " + f.path().string() + '\n').c_str());
				}
			}
			break;

			case 4:
			{
				String compute;
				if (LoadCompute(stre, compute)) {
					Shader* nShader = new Shader(0, compute.c_str());
					nShader->Pass = std::atoi(params[1].c_str());
					if (params.size() > 2) nShader->FaceCulling = std::atoi(params[2].c_str());
					if (!nShader->Success) nShader = nullptr;
					if (nShader != nullptr) {
						if (f.path().filename() == "LightCullingShader.shader") LightCullingShader = nShader;
						else Shaders.emplace(f.path().filename().replace_extension("").string(), nShader);
					}
					else throw std::exception(("Invalid shaders found! : " + f.path().string() + '\n').c_str());
				}
			}
			break;

			default:
				break;
			}

			Console::Log("Shader loaded: " + f.path().string());
			stre.close();
		}
	}
}

Material* Renderer::GetMaterialByName(String name) const
{
	if (BaseMaterials.find(name) != BaseMaterials.end()) {
		return BaseMaterials.find(name)->second;
	}
	else {
		return nullptr;
	}
}

Material* Renderer::LoadMaterialByName(String name)
{
	if (Shaders.size() < 1) return nullptr;
	if (BaseMaterials.find(name) != BaseMaterials.end()) {
		return BaseMaterials.find(name)->second;
	}
	else {
		namespace fs = std::filesystem;
		if (fs::exists(name + ".mater")) {
			INISettings params(name + ".mater");
			String shaderName = params.GetValue("Shader", "name");
			if (Shaders.find(shaderName) != Shaders.end()) {
				Material* nMat = new Material(Shaders[shaderName]);
				auto const& textures = params.GetSection("Textures");
				for (auto& [name, tex] : textures) {
					nMat->Textures.emplace(name, LoadTextureByName(tex));
				}
				auto const& scalars = params.GetSection("Scalars");
				for (auto const& [name, sca] : scalars) {
					nMat->ScalarParameters.emplace(name, std::stof(sca.c_str()));
				}
				auto const& vectors = params.GetSection("Vectors");
				for (auto const& [name, vec] : vectors) {
					nMat->VectorParameters.emplace(name, Vector(vec));
				}
				BaseMaterials.emplace(name, nMat);
				Console::Log("Material loaded: " + name);
				return BaseMaterials.find(name)->second;
			}
		}
		return nullptr;
	}
}

Texture* Renderer::LoadTextureByName(String name)
{
	if (LoadedTextures.find(name) != LoadedTextures.end()) {
		return LoadedTextures.find(name)->second;
	}
	else {
		Texture* next = nullptr;
		int width, height, nrChannels;
		if (stbi_is_hdr(name.c_str())) {
			float* data = stbi_loadf(name.c_str(), &width, &height, &nrChannels, 0);
			if (data == nullptr) return nullptr;
			next = new Texture(width, height, nrChannels, data);
			stbi_image_free(data);
		}
		else
		{
			uint8* data = stbi_load(name.c_str(), &width, &height, &nrChannels, 0);
			if (data == nullptr) return nullptr;
			int type = 0;
			if (name.rbegin()[4] == 'd' || name.rbegin()[5] == 'd') type = 0;
			else if (name.rbegin()[4] == 'n' || name.rbegin()[5] == 'n') type = 1;
			next = new Texture(width, height, nrChannels, data, type);
			stbi_image_free(data);
		}
		
		LoadedTextures.emplace(name, next);
		Console::Log("Texture loaded: " + name);
		return next;
	}
}

void Renderer::Update()
{
	if (WindowManager::GetShouldWindowClose(Window)) Exit();
}

void Renderer::EnvReflection(int width, int height)
{
	glViewport(0, 0, EnvSizeX, EnvSizeY);
	EnvironmentRender->Bind();
	glClear(GL_COLOR_BUFFER_BIT);
	glm::mat3(glm::vec3(1.0), glm::vec3(1.0), glm::vec3(1.0));
	SkyDomeShader->Bind();
	SkyDomeShader->SetUniform("P", ShadowProj);
	SkyDomeShader->SetUniform("time", WindowManager::GetWindowTime() * 1.f);
	glBindVertexArray(ScreenVao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, EnvironmentRender->GetEnvironment());
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	EnvironmentRender->BindFilter();
	SkyFilterShader->Bind();
	SkyFilterShader->SetUniform("env", 0);

	for (int i = 0; i < 1; i++) {
		SkyFilterShader->SetUniform("mip", i);
		glBindVertexArray(ScreenVao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

	glViewport(0, 0, width, height);
}

void Renderer::EnvCube(int width, int height)
{
	glDepthFunc(GL_LEQUAL);
	SkyBoxShader->Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, EnvironmentRender->GetEnvironment());
	glBindVertexArray(EnvironmentVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void Renderer::Shadows(int width, int height)
{
	/*//glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);
	glEnable(GL_DEPTH_CLAMP);

	glDepthFunc(GL_LESS);
	glDisable(GL_CULL_FACE);
	glClear(GL_STENCIL_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 0, 0xff);
	glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);

	ShadowShader->Bind();

	for (auto const& [name, s] : Shaders)
	{
		if (s != nullptr && s->Pass != 0) continue;

		for (Material* m : s->GetUsers())
		{
			if (m->GetObjects().size() < 1) continue;

			Batcher->begin();

			for (Section* o : m->GetObjects())
			{
				Batcher->add(o);
			}
			Batcher->end(1);
		}
	}
	glDisable(GL_DEPTH_CLAMP);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDisable(GL_DEPTH_TEST);

	glStencilFunc(GL_EQUAL, 1, 0xffff);
	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_KEEP);

	ShadowColorShader->Bind();
	ShadowColorShader->SetUniform("Position", 0);
	ShadowColorShader->SetUniform("numberOfTilesX", (int)WorkGroupsX);

	glBindVertexArray(ScreenVao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glDisable(GL_STENCIL_TEST);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);*/
	glViewport(0, 0, 2048, 2048);
	std::list<Section*> renders;
	for (auto const& [name, s] : Shaders)
	{
		if (s == nullptr) continue;

		for (Material* m : s->GetUsers())
		{
			if (m->GetObjects().size() < 1) continue;


			for (Section* o : m->GetObjects())
			{
				renders.push_back(o);
			}
		}
	}

	ShadowMapping->Bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_LESS);
	ShadowColorShader->Bind();
	for (const LightData* l : Lights) {
		if (l->Type != 0) continue;
		glm::vec3 loc = glm::vec3(ActiveCamera->GetLocation().X, ActiveCamera->GetLocation().Z, ActiveCamera->GetLocation().Y);
		glm::vec3 dir = glm::toMat4(glm::quat(glm::vec3(glm::radians(l->Rotation.X), glm::radians(l->Rotation.Y), glm::radians(l->Rotation.Z)))) * glm::vec4(0.0, -1.0, 0.0, 0.0);
		glm::vec3 up = glm::toMat4(glm::quat(glm::vec3(glm::radians(l->Rotation.X), glm::radians(l->Rotation.Y), glm::radians(l->Rotation.Z)))) * glm::vec4(1.0, 0.0, 0.0, 0.0);
		glm::vec3 newLoc = loc + (-dir * 70.f);
		glm::mat4 view = glm::lookAt(newLoc, newLoc + dir, up);

		ShadowColorShader->SetUniform("lSpace", ShadowOrtho * view);

		for (auto const& s : renders) {
		}
	}
	glViewport(0, 0, width, height);
}

void Renderer::SSAO(int width, int height)
{
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, SSAONoise);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, SSAOKernelBuffer);

	SSAOShader->Bind();
	SSAOShader->SetUniform("gPosition", 0);
	SSAOShader->SetUniform("gNormal", 1);
	SSAOShader->SetUniform("texNoise", 5);

	glDepthFunc(GL_ALWAYS);
	glBindVertexArray(ScreenVao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	SSAORender->BindBlur();
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, SSAORender->GetSSAO());
	SSAOBlurShader->Bind();
	SSAOBlurShader->SetUniform("ssaoInput", 5);

	glDepthFunc(GL_ALWAYS);
	glBindVertexArray(ScreenVao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
}

constexpr glm::vec4 clearClrZero(0.f);
constexpr glm::vec4 clearClrOne(1.f);

void Renderer::Forward(int width, int height)
{
	PostProcess->Bind();
	unsigned int attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	glDisable(GL_BLEND);
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_MULTISAMPLE);

	// Opaque Pass
	for (auto const& [name, s] : Shaders)
	{
		if (s == nullptr) continue;

		if (s->Pass != 0) continue;

		if (s->FaceCulling == 1) glDisable(GL_CULL_FACE);
		else glEnable(GL_CULL_FACE);

		s->Bind();

		for (Material* m : s->GetUsers())
		{
			if (m->GetObjects().size() < 1) continue;
			for (auto const& param : m->GetVectorParameters()) {
				s->SetUniform(param.first, param.second);
			}

			for (auto const& param : m->GetScalarParameters()) {
				s->SetUniform(param.first, param.second);
			}

			int round = 0;
			for (auto const& param : m->GetTextures()) {
				if (param.second > 0) {
					s->SetUniform(param.first, round);
					glActiveTexture(GL_TEXTURE0 + round);
					glBindTexture(GL_TEXTURE_2D, param.second->GetTextureID());
					round++;
				}
			}

			for (Section* o : m->GetObjects())
			{
				glm::mat4 mm = o->Parent->GetModelMatrix();
				Vector direction = ActiveCamera->GetForwardVector();
				Vector location = ActiveCamera->GetLocation();
				glm::vec3 pos = mm[3];
				glm::vec3 rad = glm::vec3((o->Parent->GetAABB().maxs - o->Parent->GetAABB().mins).Length()) * glm::mat3(mm);
				float radii = glm::max(rad.x, glm::max(rad.y, rad.z));
				glm::vec3 loc = glm::vec3(location.X, location.Z, location.Y);
				glm::vec3 dir = glm::vec3(direction.X, direction.Z, direction.Y);
				if ((loc - pos).length() > 2.f && (loc - pos).length() > radii)
				{
					if (glm::dot(dir, glm::normalize(loc - pos)) < 0.5f)
					{
						continue;
					}
				}
				s->SetUniform("Model", mm);
				o->Render();
				
			}
		}

	}
	
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunci(0, GL_ONE, GL_ONE);
	glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
	glBlendEquation(GL_FUNC_ADD);

	unsigned int attachments2[] = { GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(2, attachments2);

	glClearBufferfv(GL_COLOR, 0, &clearClrZero[0]);
	glClearBufferfv(GL_COLOR, 1, &clearClrOne[0]);

	// Translucent pass
	for (auto const& [name, s] : Shaders)
	{
		if (s == nullptr) continue;

		if (s->Pass != 1) continue;

		if (s->FaceCulling == 1) glDisable(GL_CULL_FACE);
		else glEnable(GL_CULL_FACE);

		s->Bind();

		for (Material* m : s->GetUsers())
		{
			if (m->GetObjects().size() < 1) continue;
			for (auto const& param : m->GetVectorParameters()) {
				s->SetUniform(param.first, param.second);
			}

			for (auto const& param : m->GetScalarParameters()) {
				s->SetUniform(param.first, param.second);
			}

			int round = 0;
			for (auto const& param : m->GetTextures()) {
				if (param.second > 0) {
					s->SetUniform(param.first, round);
					glActiveTexture(GL_TEXTURE0 + round);
					glBindTexture(GL_TEXTURE_2D, param.second->GetTextureID());
					round++;
				}
			}

			for (Section* o : m->GetObjects())
			{
				glm::mat4 mm = o->Parent->GetModelMatrix();
				Vector direction = ActiveCamera->GetForwardVector();
				Vector location = ActiveCamera->GetLocation();
				glm::vec3 pos = mm[3];
				glm::vec3 rad = glm::vec3(o->GetRadius()) * glm::mat3(mm);
				float radii = glm::max(rad.x, glm::max(rad.y, rad.z));
				glm::vec3 loc = glm::vec3(location.X, location.Z, location.Y);
				glm::vec3 dir = glm::vec3(direction.X, direction.Z, direction.Y);
				if ((loc - pos).length() > 2.f && (loc - pos).length() > radii)
				{
					if (glm::dot(dir, glm::normalize(loc - pos)) < 0.5f)
					{
						continue;
					}
				}
				s->SetUniform("Model", mm);
				o->Render();

			}
		}
	}

	glDrawBuffers(2, attachments);

	glDepthFunc(GL_ALWAYS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//PostProcess->Bind();

	// use composite shader
	CompositeShader->Bind();

	// draw screen quad
	PostProcess->BindTextures();
	glBindVertexArray(ScreenVao);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	PostProcess->Unbind();
}

void Renderer::PreDepth(int width, int height)
{
	glDisable(GL_BLEND);
	glDepthFunc(GL_LESS);
	// Clear the screen
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glEnable(GL_CULL_FACE);

	PreDepthShader->Bind();
	for (auto const& [name, s] : Shaders)
	{
		if (s->FaceCulling == 1) glDisable(GL_CULL_FACE);
		else glEnable(GL_CULL_FACE);

		for (Material* m : s->GetUsers())
		{
			for (Section* o : m->GetObjects())
			{
				glm::mat4 mm = o->Parent->GetModelMatrix();
				Vector direction = ActiveCamera->GetForwardVector();
				Vector location = ActiveCamera->GetLocation();
				glm::vec3 pos = mm[3];
				glm::vec3 rad = glm::vec3(o->GetRadius()) * glm::mat3(mm);
				float radii = glm::max(rad.x, glm::max(rad.y, rad.z));
				glm::vec3 loc = glm::vec3(location.X, location.Z, location.Y);
				glm::vec3 dir = glm::vec3(direction.X, direction.Z, direction.Y);
				if ((loc - pos).length() > 2.f && (loc - pos).length() > radii)
				{
					if (glm::dot(dir, glm::normalize(loc - pos)) < 0.65f)
					{
						continue;
					}
				}
				PreDepthShader->SetUniform("Model", mm);
				o->Render();

			}
		}
	}
}

void Renderer::LightCulling(int width, int height)
{
	LightCullingShader->Bind();

	LightCullingShader->SetUniform("depthMap", 4);

	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, LightBuffer);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, VisibleLightIndicesBuffer);

	glDispatchCompute(WorkGroupsX, WorkGroupsY, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void Renderer::UpdateTransforms() {
	for (auto const& [name, s] : Shaders)
	{
		if (s->FaceCulling == 1) glDisable(GL_CULL_FACE);
		else glEnable(GL_CULL_FACE);

		for (Material* m : s->GetUsers())
		{
			for (Section* o : m->GetObjects())
			{
				o->Parent->ApplyTransform();
			}
		}
	}
}

void Renderer::Render(float delta)
{
	if (ActiveCamera == nullptr) return;

	Vector2D size = WindowManager::GetWindowSize(Window);
	int width = size.X;
	int height = size.Y;

	UpdateTransforms();

	GlobalVariables.Projection = ActiveCamera->GetProjectionMatrix();
	GlobalVariables.View = glm::inverse(ActiveCamera->GetViewMatrix());
	const Vector& loc = ActiveCamera->GetLocation();
	GlobalVariables.ViewPoint = glm::vec4(loc.X, loc.Z, loc.Y, 1.f);
	GlobalVariables.ScreenSize = glm::ivec2(width, height);
	GlobalVariables.SceneLightCount = (int)Lights.size();

	glBindBuffer(GL_UNIFORM_BUFFER, GlobalUniforms);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Globals), &GlobalVariables);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, GlobalUniforms);


	if (Lights.size() != 0) UpdateLights();
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, LightBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, VisibleLightIndicesBuffer);

	//EnvReflection(width, height);

	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);
	DepthBuffer->Bind();

	PreDepth(width, height);

	DepthBuffer->BindTextures();
	LightCulling(width, height);

	Forward(width, height);

	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//DepthBuffer->BindTextures();
	EnvironmentRender->BindTextures();

	/*glDepthFunc(GL_ALWAYS);
	glBindVertexArray(ScreenVao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);*/

	/*glBindFramebuffer(GL_READ_FRAMEBUFFER, DepthBuffer->GetBuffer());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, PostProcess->GetBuffer());
	glBlitFramebuffer(
		0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST
	);*/

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, 0);

	//EnvCube(width, height);

	BlurRender->Blur(PostProcess->GetBloom(), 4, ScreenVao);
	PostProcess->BindTextures();

	PostProcessMaster->Bind();

	glDepthFunc(GL_ALWAYS);
	glBindVertexArray(ScreenVao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	UIHolder->Render(0);

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, 0);

	float fps = 1.f / delta;
	char sbuffer[32];
	if (fpsCounter) {
		sprintf_s(sbuffer, 32, "Eril Engine Demo | FPS: %.1f", fps);
		WindowManager::SetWindowTitle(Window, sbuffer);
	}
	WindowManager::UpdateWindow(Window);
}

void Renderer::GameStart()
{
	WindowManager::SetWindowTitle(Window, "Eril Engine Demo");
}

void Renderer::DestroyWindow()
{
	WindowManager::CloseWindow(Window);
}

GLMesh::GLMesh()
{
	ActiveDir = "";
}

GLMesh::~GLMesh()
{
	for (auto const& i : LoadedMeshes) {
		delete i.second;
	}
	ModelStreams.clear();
}

void processMesh(LoadedMesh* meshHolder, aiMesh* mesh)
{
	std::vector<Vertex> vertices;
	std::vector<uint32> indices;
	float radius = 0.f;
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;
		if (radius < mesh->mVertices[i].x) radius = mesh->mVertices[i].x;

		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;

		vertex.tangent.x = mesh->mTangents[i].x;
		vertex.tangent.y = mesh->mTangents[i].y;
		vertex.tangent.z = mesh->mTangents[i].z;

		if (mesh->HasTextureCoords(0))
		{
			vertex.uv.x = mesh->mTextureCoords[0][i].x;
			vertex.uv.y = mesh->mTextureCoords[0][i].y;
		}
		else {
			vertex.uv.x = 0;
			vertex.uv.y = 0;
		}

		vertices.push_back(vertex);
	}
	//Now retrieve the corresponding vertex indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}
	std::vector<uint> adjacent;
	MeshDataHolder* section = new MeshDataHolder(vertices.data(), vertices.size(), indices.data(), indices.size());
	section->Radius = radius;
	/*section->FaceCount = indices.size() / 3;
	section->VertexCount = vertices.size();


	section->indices = new uint32[indices.size() * sizeof(uint32)];
	memcpy(section->indices, indices.data(), indices.size() * sizeof(uint32));

	section->vertices = new Vertex[vertices.size() * sizeof(Vertex)];
	memcpy(section->vertices, vertices.data(), vertices.size() * sizeof(Vertex));*/

	section->Instance = RI->LoadMaterialByName("Assets/Materials/default");

	meshHolder->HolderCount++;
	meshHolder->Holders.push_back(section);
}

void processNode(LoadedMesh* mesh, aiNode* node, const aiScene* scene)
{
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		// the node object only contains indices to index the actual objects in the scene.
		aiMesh* meshed = scene->mMeshes[node->mMeshes[i]];
		processMesh(mesh, meshed);
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(mesh, node->mChildren[i], scene);
	}
}

LoadedMesh* loadMeshes(const std::string& path)
{
	LoadedMesh* mesh = new LoadedMesh();
	//read file with Assimp
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);
	//Check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		Console::Log("Error loading model file " + path + " : " + importer.GetErrorString());
		delete mesh;
		return nullptr;
	}
	// retrieve the directory path of the filepath
	std::string dir = path.substr(0, path.find_last_of('/'));

	processNode(mesh, scene->mRootNode, scene);

	return mesh;
}

RenderMesh* GLMesh::LoadData(VisibleObject* parent, String name)
{
	auto result = LoadedMeshes.find(name);
	if (result == LoadedMeshes.end()) {
		auto r = ModelStreams.find(name);
		if (r == ModelStreams.end()) return nullptr;

		LoadedMesh* mesh = loadMeshes(r->second);
		LoadedMeshes.emplace(name, mesh);

		/*mesh->HolderCount = 1;
		mesh->Holders = new MeshDataHolder[mesh->HolderCount]();
		std::ifstream* s = r->second;
		s->seekg(0);
		String in;
		size_t v_index = 0;
		size_t n_index = 0;
		std::vector<float> verts;
		std::vector<float> normals;
		std::vector<unsigned int> normalIndices;
		std::vector<unsigned int> faces;
		for (uint32 i = 0; i < mesh->HolderCount; i++) {
			MeshDataHolder* hold = &mesh->Holders[i];
			while (std::getline(*s, in)) {
				switch (in.front())
				{
				case 'v':
				{
					std::vector<String> vec = split(in, ' ');
					if (vec[0] == "v") {
						verts.push_back(std::stof(vec[1]));
						verts.push_back(std::stof(vec[2]));
						verts.push_back(std::stof(vec[3]));
						v_index++;
					}
					else if (vec[0] == "vn") {
						normals.push_back(std::stof(vec[1]));
						normals.push_back(std::stof(vec[2]));
						normals.push_back(std::stof(vec[3]));
						n_index++;
					}
				}
				break;

				case 'f':
				{
					std::vector<String> face = split(in, ' ');
					face.erase(face.begin(), face.begin() + 1);
					for (String str : face) {
						std::vector<String> data = split(str, '/');
						faces.push_back(std::stoi(data[0]) - 1);
						normalIndices.push_back(std::stoi(data[2]) - 1);
					}
				}
				break;

				default:
					break;
				}
			}
			hold->FaceCount = (uint32)faces.size() / 3;
			hold->VertexCount = (uint32)v_index;
			hold->vertices = new Vertex[v_index]();
			hold->indices = new uint32[faces.size()]();

			for (unsigned int i = 0; i < v_index; i++) {
				hold->vertices[i].position.x = verts[i * 3];
				hold->vertices[i].position.y = verts[i * 3 + 1];
				hold->vertices[i].position.z = verts[i * 3 + 2];
			}

			for (uint i = 0; i < faces.size(); i++) {
				hold->vertices[faces[i]].normal.x = normals[normalIndices[i] * 3 + 0];
				hold->vertices[faces[i]].normal.y = normals[normalIndices[i] * 3 + 1];
				hold->vertices[faces[i]].normal.z = normals[normalIndices[i] * 3 + 2];
			}

			memcpy(hold->indices, faces.data(), faces.size() * sizeof(uint32));

			hold->Instance = RI->LoadMaterialByName("Shaders/test");
		}*/


		RenderObject* obj = new RenderObject(mesh);
		obj->SetParent(parent);
		return obj;
	}
	else 
	{
		RenderObject* obj = new RenderObject(result->second);
		obj->SetParent(parent);
		return obj;
	}
}

RenderMesh* GLMesh::CreateProcedural(VisibleObject* parent, String name, std::vector<Vector>& positions, std::vector<Vector> UV, std::vector<Vector>& normal, std::vector<Vector>& tangent, std::vector<uint32>& indices)
{
	LoadedMesh* mesh = new LoadedMesh();

	std::vector<Vertex> verts;
	verts.resize(positions.size());

	AABB bounds;
	for (auto i = 0; i < positions.size(); i++) {
		if		(bounds.mins.X > positions[i].X) bounds.mins.X = positions[i].X;
		else if (bounds.maxs.X < positions[i].X) bounds.maxs.X = positions[i].X;
		if		(bounds.mins.Y > positions[i].Y) bounds.mins.Y = positions[i].Y;
		else if (bounds.maxs.Y < positions[i].Y) bounds.maxs.Y = positions[i].Y;
		if		(bounds.mins.Z > positions[i].Z) bounds.mins.Z = positions[i].Z;
		else if (bounds.maxs.Z < positions[i].Z) bounds.maxs.Z = positions[i].Z;

		verts[i].position = glm::vec3(positions[i].X, positions[i].Z, positions[i].Y);
		verts[i].uv = glm::vec3(UV[i].X, UV[i].Y, UV[i].Z);
		verts[i].normal = glm::vec3(normal[i].X, normal[i].Z, normal[i].Y);
		verts[i].tangent = glm::vec3(tangent[i].X, tangent[i].Z, tangent[i].Y);
	}

	MeshDataHolder* section = new MeshDataHolder(verts.data(), positions.size(), indices.data(), indices.size());

	mesh->HolderCount++;
	mesh->Holders.push_back(section);
	section->Radius = bounds.maxs.Length();

	LoadedMeshes.emplace(name, mesh);
	RenderObject* obj = new RenderObject(mesh);
	obj->SetParent(parent);
	obj->SetAABB(bounds);
	return obj;
}

void GLMesh::StartLoading()
{
	namespace fs = std::filesystem;
	ActiveDir = INI->GetValue("Engine", "DataFolder") + "/Meshes";
	for (const auto& f : fs::recursive_directory_iterator(ActiveDir)) {
		if (f.path().extension() == ".obj") {
			ModelStreams[f.path().filename().replace_extension("").string()] = f.path().string();
		}
	}
}
