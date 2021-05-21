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

struct Globals
{
	glm::mat4 Projection;
	glm::mat4 View;
	glm::vec4 ViewPoint;
};

struct GLM_Light
{
	glm::vec4 location;
	float size;
};

Renderer::Renderer()
{
	Batcher = nullptr;
	Buffer = nullptr;
	Window = nullptr;
	ActiveCamera = nullptr;
	DeferredMaster = nullptr;
	LightCullingShader = nullptr;
	MaxLightCount = 1024;

	ScreenVao = 0;
	ScreenVbo = 0;
	ScreenTexVbo = 0;
}

Renderer::~Renderer()
{
	if (Batcher != nullptr) delete Batcher;
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
	glDepthFunc(GL_EQUAL);
	glEnable(GL_CULL_FACE);

	glViewport(0, 0, width, height);
	glClearColor(0.f, 0.f, 0.f, 0.f);

	LoadShaders();

	if (LightCullingShader == nullptr || DeferredMaster == nullptr) throw std::exception("Shaders not found!\n");

	LightCullingShader->Bind();
	LightCullingShader->SetUniform("screenSize", width, height);
	LightCullingShader->SetUniform("lightCount", (int)Lights.size());
	DeferredMaster->Bind();
	DeferredMaster->SetUniform("numberOfTilesX", (int)WorkGroupsX);

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

	delete Batcher;
	delete Buffer;
	delete DeferredMaster;
	delete LightCullingShader;

	glDeleteBuffers(1, &LightBuffer);
	glDeleteBuffers(1, &VisibleLightIndicesBuffer);
	glDeleteBuffers(1, &GlobalUniforms);

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
		GLM_Light& light = mapped[i];
		light.location = glm::vec4(Lights[i]->Location.X, Lights[i]->Location.Y, Lights[i]->Location.Z, 1.f);
		light.size = Lights[i]->Size;
	}
	int result = glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	if (result == GL_FALSE) exit(99);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

bool LoadVertex(std::ifstream& s, String& vertex)
{
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

bool LoadFragment(std::ifstream& s, String& fragment)
{
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
			auto stream = std::ifstream(name + ".mater", std::ios_base::in);
			String shaderName;
			std::getline(stream, shaderName);
			if (Shaders.find(shaderName) != Shaders.end()) {
				Material* nMat = new Material(Shaders[shaderName]);
				BaseMaterials.emplace(name, nMat);
				return BaseMaterials.find(name)->second;
			}
		}
		return nullptr;
	}
}

void Renderer::Update()
{
	glfwPollEvents();
	if (glfwWindowShouldClose(Window)) Exit();
}

void Renderer::Deferred(int width, int height)
{
	Buffer->Bind();
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_BLEND);
	glDepthFunc(GL_LESS);

	for (auto const& [name, s] : Shaders)
	{
		if (s->Pass != 0) continue;

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

	Buffer->Unbind();
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
	glDepthFunc(GL_EQUAL);
	LightCullingShader->Bind();

	glActiveTexture(GL_TEXTURE4);
	LightCullingShader->SetUniform("depthMap", 4);
	glBindTexture(GL_TEXTURE_2D, Buffer->GetDepth());

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, LightBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, VisibleLightIndicesBuffer);
	LightCullingShader->SetUniform("lightCount", (int)Lights.size());

	glDispatchCompute(WorkGroupsX, WorkGroupsY, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void LightStencilPass(int i) 
{
	glDrawBuffer(GL_NONE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glClear(GL_STENCIL_BUFFER_BIT);
	glStencilFunc(GL_ALWAYS, 0, 0);
	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
	glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
	glEnable(GL_STENCIL_TEST);


}

void LightPointPass(int i)
{
	glStencilFunc(GL_NOTEQUAL, 0, 0xFF);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);



	glCullFace(GL_BACK);

	glDisable(GL_BLEND);
}

void LightDirectionalPass()
{

}

void Renderer::DeferredLighting(int width, int height)
{
	for (unsigned int i = 0; i < Lights.size(); i++) {
		LightStencilPass(i);
		LightPointPass(i);
	}

	glDisable(GL_STENCIL_TEST);

	LightDirectionalPass();
}

void Renderer::Render()
{
	int width, height;

	if (ActiveCamera == nullptr) return;

	glfwGetFramebufferSize(Window, &width, &height);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Globals g;
	g.Projection = ActiveCamera->GetProjectionMatrix();
	g.View = glm::inverse(ActiveCamera->GetViewMatrix());
	const Vector& loc = ActiveCamera->GetLocation();
	g.ViewPoint = glm::vec4(loc.X, loc.Y, loc.Z, 1.f);

	glBindBuffer(GL_UNIFORM_BUFFER, GlobalUniforms);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Globals), &g);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, GlobalUniforms);

	if (Lights.size() != 0) UpdateLights();
	Deferred(width, height);

	LightCulling(width, height);
	DeferredLighting(width, height);

	Buffer->Unbind();
	Buffer->BindTextures();
	DeferredMaster->Bind();
	//DeferredMaster->SetUniform("numberOfTilesX", (int)WorkGroupsX);
	DeferredMaster->SetUniform("gPosition", 0);
	DeferredMaster->SetUniform("gNormal", 1);
	DeferredMaster->SetUniform("gAlbedoSpec", 2);
	//DeferredMaster->SetUniform("totalLightCount", (int)Lights.size());

	glBindVertexArray(ScreenVao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, Buffer->GetBuffer());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(
		0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST
	);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, 0);

	Forward(width, height);

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, 0);

	glfwSwapBuffers(Window);
}

GLMesh::GLMesh()
{
	ActiveDir = "";
}

GLMesh::~GLMesh()
{
	for (auto const& i : ModelStreams) {
		i.second->close();
	}
	ModelStreams.clear();
}

RenderMesh* GLMesh::LoadData(VisibleObject* parent, String name)
{
	auto result = LoadedMeshes.find(name);
	if (result == LoadedMeshes.end()) {
		auto r = ModelStreams.find(name);
		if (r == ModelStreams.end()) return nullptr;

		LoadedMesh* mesh = new LoadedMesh();
		LoadedMeshes.emplace(name, mesh);

		mesh->HolderCount = 1;
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
		}
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
			ModelStreams[f.path().filename().replace_extension("").string()] = stre;
		}
	}
}
