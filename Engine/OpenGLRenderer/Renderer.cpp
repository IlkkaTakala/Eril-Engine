#include "Material.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "Core.h"
#include "Renderer.h"
#include "Batcher.h"
#include "Camera.h"
#include "Mesh.h"
#include "Texture.h"
#include "RenderBuffer.h"
#include "../Objects/VisibleObject.h"
#include "LightData.h"
#include "Settings.h"
#include <filesystem>
#include <stdexcept>
#include <iostream>
#include <random>

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
	glm::mat4 transforms[6];

	GLM_Light() {
		color = glm::vec4(0.0);
		locationAndSize = glm::vec4(0.0);
		rotation = glm::vec4(0.0);
		type = glm::ivec4(1);
		transforms[0] = { glm::mat4(1.0) };
	}
};

Renderer::Renderer()
{
	Batcher = nullptr;
	Buffer = nullptr;
	BlurRender = nullptr;
	Window = nullptr;
	ActiveCamera = nullptr;
	DeferredMaster = nullptr;
	LightCullingShader = nullptr;
	SSAOShader = nullptr;
	SSAOBlurShader = nullptr;
	ShadowShader = nullptr;

	MaxLightCount = 1024;

	ScreenVao = 0;
	ScreenVbo = 0;
	ScreenTexVbo = 0;
}

Renderer::~Renderer()
{
	if (Batcher != nullptr) delete Batcher;
}

inline float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

int Renderer::SetupWindow(int width, int height)
{
	//glfwSetErrorCallback([](int error, const char* description) {
	//	fprintf(stderr, "Error %d: %s\n", error, description);
	//});

	if (!glfwInit()) {
		return -1;
	}

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	Window = glfwCreateWindow(width, height, "OpenGL window", NULL, NULL);
	if (!Window) {
		glfwTerminate();
		return -1;
	}
	glfwSetWindowAttrib(Window, GLFW_DECORATED, GLFW_FALSE);
	glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetInputMode(Window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(Window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	glfwMakeContextCurrent(Window);
	if (INI->GetValue("Render", "VSync") == "false") glfwSwapInterval(0);
	if (!gladLoadGL(glfwGetProcAddress)) exit(100);

	Batcher = new RenderBatch(/*262144*/524288);
	Buffer = new RenderBuffer(width, height);
	PostProcess = new PostBuffer(width, height);
	SSAORender = new SSAOBuffer(width, height);

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

	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then 
	// configure vertex attributes(s).
	glBindVertexArray(ScreenVao);

	// Set buffer data to m_vbo-object (bind buffer first and then set the data)
	glBindBuffer(GL_ARRAY_BUFFER, ScreenVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Set buffer data to m_texCoordsVbo-object (bind buffer first and then set the data)
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

	LoadShaders();

	if (LightCullingShader == nullptr || DeferredMaster == nullptr) throw std::exception("Important shaders not found!\n");

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
	ShadowRender = new ShadowBuffer(SHADOW_WIDTH, SHADOW_HEIGHT);

	float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
	float near = 0.1f;
	float far = 100.0f;
	ShadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far);
	ShadowOrtho = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near, far);

	return 0;
}

void Renderer::CleanRenderer()
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

	delete Batcher;
	delete Buffer;
	delete PostProcess;
	delete BlurRender;
	delete DeferredMaster;
	delete PostProcessMaster;
	delete LightCullingShader;
	delete SSAOShader;
	delete ShadowShader;
	delete ShadowRender;

	glDeleteBuffers(1, &LightBuffer);
	glDeleteBuffers(1, &VisibleLightIndicesBuffer);
	glDeleteBuffers(1, &GlobalUniforms);
	glDeleteBuffers(1, &SSAOKernelBuffer);
	glDeleteTextures(1, &SSAONoise);

	glDeleteBuffers(1, &ScreenVbo);
	glDeleteBuffers(1, &ScreenTexVbo);
	glDeleteVertexArrays(1, &ScreenVao);

	glfwDestroyWindow(Window);
	glfwTerminate();
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
		light.color = glm::vec4(Lights[i]->Color.X, Lights[i]->Color.Y, Lights[i]->Color.Z, 1.0);
		light.type.x = Lights[i]->Type;
		light.transforms[0] = ShadowProj * glm::lookAt(glm::vec3(light.locationAndSize), glm::vec3(light.locationAndSize) + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
		light.transforms[1] = ShadowProj * glm::lookAt(glm::vec3(light.locationAndSize), glm::vec3(light.locationAndSize) + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
		light.transforms[2] = ShadowProj * glm::lookAt(glm::vec3(light.locationAndSize), glm::vec3(light.locationAndSize) + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 1.0));
		light.transforms[3] = ShadowProj * glm::lookAt(glm::vec3(light.locationAndSize), glm::vec3(light.locationAndSize) + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 1.0, -1.0));
		light.transforms[4] = ShadowProj * glm::lookAt(glm::vec3(light.locationAndSize), glm::vec3(light.locationAndSize) + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));
		light.transforms[5] = ShadowProj * glm::lookAt(glm::vec3(light.locationAndSize), glm::vec3(light.locationAndSize) + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));
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
	String folder = INI->GetValue("Engine", "ShaderFolder");
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
					if (!nShader->Success) nShader = nullptr;
					if (nShader != nullptr) {
						if (f.path().filename() == "Master.shader") DeferredMaster = nShader;
						else if (f.path().filename() == "PostProcessMaster.shader") PostProcessMaster = nShader;
						else if (f.path().filename() == "SSAO.shader") SSAOShader = nShader;
						else if (f.path().filename() == "SSAOBlur.shader") SSAOBlurShader = nShader;
						else Shaders.emplace(f.path().filename().replace_extension("").string(), nShader);
					}
					else throw std::exception(("Invalid shaders found! : " + f.path().string() + '\n').c_str());
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
					if (!nShader->Success) nShader = nullptr;
					if (nShader != nullptr) {
						if (f.path().filename() == "Shadow.shader") ShadowShader = nShader;
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
			//auto stream = std::ifstream(name + ".mater", std::ios_base::in);
			INISettings params(name + ".mater");
			String shaderName = params.GetValue("Shader", "name");
			//std::getline(stream, shaderName);
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
		int width, height, nrChannels;
		float* data = stbi_loadf(name.c_str(), &width, &height, &nrChannels, 0);
		if (data == nullptr) return nullptr;
		int type = name.rbegin()[4] == 'd' ? 0 : name.rbegin()[4] == 'n' ? 1 : 2;
		Texture* next = new Texture(width, height, nrChannels, data, type);
		stbi_image_free(data);
		LoadedTextures.emplace(name, next);
		return next;
	}
}

void Renderer::Update()
{
	glfwPollEvents();
	if (glfwWindowShouldClose(Window)) Exit();
}

void Renderer::Shadows(int width, int height)
{
	glViewport(0, 0, width, height);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	ShadowShader->Bind();

	for (auto const& [name, s] : Shaders)
	{
		if (s != nullptr && s->Pass != 0) continue;

		if (s == nullptr) throw std::exception("Shader error!");

		for (Material* m : s->GetUsers())
		{

			Batcher->begin();

			for (Section* o : m->GetObjects())
			{
				Batcher->add(o);
			}
			Batcher->end(ActiveCamera->GetViewMatrix(), ActiveCamera->GetProjectionMatrix());
		}
	}
}

void Renderer::Deferred(int width, int height)
{
	// Clear the screen
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);

	for (auto const& [name, s] : Shaders)
	{
		if (s != nullptr && s->Pass != 0) continue;

		if (s == nullptr) throw std::exception("Shader error!");
		s->Bind();

		//s->SetUniform("VP", ActiveCamera->GetProjectionMatrix() * glm::inverse(ActiveCamera->GetViewMatrix()));

		for (Material* m : s->GetUsers())
		{
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

			Batcher->begin();

			for (Section* o : m->GetObjects())
			{
				Batcher->add(o);
			}
			Batcher->end(ActiveCamera->GetViewMatrix(), ActiveCamera->GetProjectionMatrix());
		}
	}
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

void Renderer::Forward(int width, int height)
{
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (auto const& [name, s] : Shaders)
	{
		if (s == nullptr) throw std::exception("Shader error!");
		if (s->Pass != 1) continue;
		if (s->GetUsers().size() == 0) continue;
		s->Bind();

		s->SetUniform("VP", ActiveCamera->GetProjectionMatrix() * glm::inverse(ActiveCamera->GetViewMatrix()));

		for (Material* m : s->GetUsers())
		{
			for (auto const& param : m->GetVectorParameters()) {
				s->SetUniform(param.first, param.second);
			}

			for (auto const& param : m->GetScalarParameters()) {
				s->SetUniform(param.first, param.second);
			}

			int round = 0;
			for (auto const& param : m->GetTextures()) {
				if (param.second > 0) {
					s->SetUniform(param.first, static_cast<float>(param.second->GetTextureID()));
					glActiveTexture(GL_TEXTURE0 + round);
					glBindTexture(GL_TEXTURE_2D, param.second->GetTextureID());
					round++;
				}
			}

			Batcher->begin();

			for (Section* o : m->GetObjects())
			{
				Batcher->add(o);
			}
			Batcher->end(ActiveCamera->GetViewMatrix(), ActiveCamera->GetProjectionMatrix());
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

void Renderer::Render()
{
	int width, height;

	if (ActiveCamera == nullptr) return;

	glfwGetFramebufferSize(Window, &width, &height);

	GlobalVariables.Projection = ActiveCamera->GetProjectionMatrix();
	GlobalVariables.View = glm::inverse(ActiveCamera->GetViewMatrix());
	const Vector& loc = ActiveCamera->GetLocation();
	GlobalVariables.ViewPoint = glm::vec4(loc.X, loc.Y, loc.Z, 1.f);
	GlobalVariables.ScreenSize = glm::ivec2(width, height);
	GlobalVariables.SceneLightCount = (int)Lights.size();

	glBindBuffer(GL_UNIFORM_BUFFER, GlobalUniforms);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Globals), &GlobalVariables);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, GlobalUniforms);

	if (Lights.size() != 0) UpdateLights();
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, LightBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, VisibleLightIndicesBuffer);

	/*ShadowRender->Bind();
	Shadows(1024, 1024);*/

	glViewport(0, 0, width, height);
	Buffer->Bind();

	Deferred(width, height);
	Buffer->BindTextures();
	SSAORender->Bind();
	SSAO(width, height);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, LightBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, VisibleLightIndicesBuffer);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, SSAORender->GetSSAOBlur());
	LightCulling(width, height);

	PostProcess->Bind();
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ShadowRender->BindTextures();

	DeferredMaster->Bind();
	DeferredMaster->SetUniform("numberOfTilesX", (int)WorkGroupsX);
	DeferredMaster->SetUniform("gPosition", 0);
	DeferredMaster->SetUniform("gNormal", 1);
	DeferredMaster->SetUniform("gAlbedoSpec", 2);
	DeferredMaster->SetUniform("gData", 3);
	DeferredMaster->SetUniform("gDepth", 4);
	DeferredMaster->SetUniform("gSSAO", 5);
	DeferredMaster->SetUniform("gShadow", 0);

	glDepthFunc(GL_ALWAYS);
	glBindVertexArray(ScreenVao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, Buffer->GetBuffer());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, PostProcess->GetBuffer());
	glBlitFramebuffer(
		0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST
	);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, 0);

	//Forward(width, height);

	PostProcess->Unbind();
	BlurRender->Blur(PostProcess->GetBloom(), 5);
	PostProcess->BindTextures();

	PostProcessMaster->Bind();
	PostProcessMaster->SetUniform("Color", 0);
	PostProcessMaster->SetUniform("Bloom", 1);

	glDepthFunc(GL_ALWAYS);
	glBindVertexArray(ScreenVao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, 0);

	glfwSwapBuffers(Window);
}

GLMesh::GLMesh()
{
	ActiveDir = "";
}

GLMesh::~GLMesh()
{
	/*for (auto const& i : ModelStreams) {
		i.second->close();
	}*/
	ModelStreams.clear();
}

void processMesh(LoadedMesh* meshHolder, aiMesh* mesh)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned> indices;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

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
	MeshDataHolder* section = new MeshDataHolder();
	section->FaceCount = indices.size() / 3;
	section->VertexCount = vertices.size();
	section->indices = new uint32[indices.size() * sizeof(uint32)];
	memcpy(section->indices, indices.data(), indices.size() * sizeof(uint32));

	section->vertices = new Vertex[vertices.size() * sizeof(Vertex)];
	memcpy(section->vertices, vertices.data(), vertices.size() * sizeof(Vertex));

	section->Instance = RI->LoadMaterialByName("Shaders/test");

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
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	//Check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		printf("Error loading model file \"%s\": \"%s\" ", path.c_str(), importer.GetErrorString());
		return mesh;
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

void GLMesh::StartLoading()
{
	namespace fs = std::filesystem;
	ActiveDir = INI->GetValue("Engine", "DataFolder");
	for (const auto& f : fs::recursive_directory_iterator(ActiveDir)) {
		if (f.path().extension() == ".obj") {
			std::ifstream* stre = new std::ifstream(f.path(), std::ios_base::in);
			//ModelStreams[f.path().filename().replace_extension("").string()] = stre;
			ModelStreams[f.path().filename().replace_extension("").string()] = f.path().string();
		}
	}
}
