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

Renderer::Renderer()
{
	Batcher = nullptr;
	Buffer = nullptr;
	Window = nullptr;
	ActiveCamera = nullptr;
	DeferredMaster = nullptr;

	ScreenPlane = 0;
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
	Window = glfwCreateWindow(width, height, "OpenGL window", NULL, NULL);
	if (!Window) {
		glfwTerminate();
		return -1;
	}
	glfwSetWindowAttrib(Window, GLFW_RESIZABLE, GLFW_FALSE);
	glfwSetWindowAttrib(Window, GLFW_DECORATED, GLFW_FALSE);
	glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetInputMode(Window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(Window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	glfwMakeContextCurrent(Window);
	if (INI->GetValue("Render", "VSync") == "false") glfwSwapInterval(0);
	if (!gladLoadGL(glfwGetProcAddress)) exit(100);

	Batcher = new RenderBatch(262144);
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

	glGenVertexArrays(1, &ScreenPlane);
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

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);

	glViewport(0, 0, width, height);
	glClearColor(0.f, 0.f, 0.f, 1.0f);

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
}

void Renderer::LoadShaders()
{
	namespace fs = std::filesystem;
	String folder = INI->GetValue("Engine", "ShaderFolder");
	for (const auto& f : fs::recursive_directory_iterator(folder)) {
		if (f.path().extension() == ".shader") {
			std::ifstream stre = std::ifstream(f.path(), std::ios_base::in);

			String vertex;
			String fragment;
			String in_line;

			std::getline(stre, in_line);

			std::vector<String> params = split(in_line, ';');

			do {
				std::getline(stre, in_line);
			} while (in_line != "###Vertex###");

			while (std::getline(stre, in_line)) {
				if (in_line != "###Fragment###")
					vertex += in_line + '\n';
				else break;
			}

			while (std::getline(stre, in_line)) {
				if (in_line != "###Fragment###")
					fragment += in_line + '\n';
				else break;
			}

			Shader* nShader = new Shader(vertex.c_str(), fragment.c_str());
			nShader->Pass = std::atoi(params[0].c_str());
 			if (!nShader->Success) nShader = nullptr;
			if (nShader != nullptr) {
				if (f.path().filename() == "Master.shader") DeferredMaster = nShader;
				else Shaders.emplace(f.path().filename().replace_extension("").string(), nShader);
			}
			else throw std::exception("Invalid shaders found!\n");

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
	glClearColor(0.f, 0.f, 0.f, 0.f);
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_BLEND);

	for (auto const& [name, s] : Shaders)
	{
		if (s->Pass != 0) continue;

		if (s == nullptr) throw std::exception("Shader error!");
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

	Buffer->Unbind();

	glfwGetFramebufferSize(Window, &width, &height);

	Buffer->BindTextures();
	DeferredMaster->Bind();
	DeferredMaster->SetUniform("gPosition", 0);
	DeferredMaster->SetUniform("gNormal", 1);
	DeferredMaster->SetUniform("gAlbedoSpec", 2);

	glBindVertexArray(ScreenVao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, Buffer->GetBuffer());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(
		0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST
	);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::Forward(int width, int height)
{
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

void Renderer::Render()
{
	int width, height;

	if (ActiveCamera == nullptr) return;

	glfwGetFramebufferSize(Window, &width, &height);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Deferred(width, height);

	Forward(width, height);

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
