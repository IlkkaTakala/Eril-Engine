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
#include "Settings.h"
#include <filesystem>
#include <sstream>
#include <iterator>
#include <stdexcept>

Renderer::Renderer()
{
	Batcher = nullptr;
	Buffer = nullptr;
	Window = nullptr;
	ActiveCamera = nullptr;

	ScreenPlane = 0;
	ScreenVao = 0;
	ScreenVbo = 0;
	ScreenTexVbo = 0;
}

Renderer::~Renderer()
{
	if (Batcher != nullptr) delete Batcher;
}

int Renderer::SetupWindow()
{
	//glfwSetErrorCallback([](int error, const char* description) {
	//	fprintf(stderr, "Error %d: %s\n", error, description);
	//});

	if (!glfwInit()) {
		return -1;
	}
	Window = glfwCreateWindow(640, 480, "OpenGL window", NULL, NULL);
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
	Buffer = new RenderBuffer(640, 480);


	float vertices[] = {
		-1.f, -1.f, 0.f,
		1.f, -1.f, 0.f,
		1.f, 1.f, 0.f,
		-1.f, -1.f, 0.f,
		1.f, 1.f, 0.f,
		-1.f, 1.f, 0.f
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
 			if (!nShader->Success) nShader = nullptr;
			if (nShader != nullptr) Shaders.emplace(f.path().filename().replace_extension("").string(), nShader);
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
		Material* nMat = new Material(Shaders.find("shader_001")->second);
		BaseMaterials.emplace(name, nMat);
		return BaseMaterials.find(name)->second;;
	}
}

void Renderer::Update()
{
	glfwPollEvents();
	if (glfwWindowShouldClose(Window)) Exit();
}

void Renderer::Render()
{
	int width, height;

	Buffer->Bind();

	glfwGetFramebufferSize(Window, &width, &height);

	// Setup the opengl wiewport (i.e specify area to draw)
	glViewport(0, 0, width, height);
	// Set color to be used when clearing the screen
	glClearColor(0.1f, 0.1f, 0.1f, 0.f);
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (ActiveCamera == nullptr) return;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);

	for (auto const& map : Shaders)
	{
		Shader* s = map.second;
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

	glViewport(0, 0, width, height);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Buffer->BindTextures();
	Shaders["Master"]->Bind();
	Shaders["Master"]->SetUniform("gPosition", 0);
	Shaders["Master"]->SetUniform("gNormal", 1);
	Shaders["Master"]->SetUniform("gAlbedoSpec", 2);

	glBindVertexArray(ScreenVao);
	glDrawArrays(GL_TRIANGLES, 0, 6);

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

template <typename Out>
void split(const String& s, char delim, Out result) {
	std::istringstream iss(s);
	String item;
	while (std::getline(iss, item, delim)) {
		*result++ = item;
	}
}

std::vector<String> split(const String& s, char delim) {
	std::vector<String> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
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

			hold->Instance = RI->LoadMaterialByName("test");
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
