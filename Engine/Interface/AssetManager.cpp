#include "AssetManager.h"
#include <RenderCore/RenderCommands.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Mesh.h>
#include <Texture.h>
#include <Basic/QueueSafe.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace AssetManager
{
	namespace {

		std::thread* LoaderThread;

		std::unordered_map<String, LoadedMesh*> StaticMeshes;
		std::map<String, Texture*> LoadedTextures;

		SafeQueue<std::pair<String, void*>> LoadQueue;
		SafeQueue<std::pair<String, void*>> UnloadQueue;
	}
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
		vertex.position.y = -mesh->mVertices[i].z;
		vertex.position.z = mesh->mVertices[i].y;
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
	MeshDataHolder* section = new MeshDataHolder(vertices.data(), (uint32)vertices.size(), indices.data(), (uint32)indices.size());
	section->Radius = radius;

	section->DefaultMaterial = IRender::LoadMaterialByName("Assets/Materials/default");

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
	const aiScene* scene = importer.ReadFile(path + ".obj", aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);
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

namespace AssetManager
{
	namespace {
		void TextureLoader(const String& name, Texture* ptr)
		{
			Texture* next = ptr ? ptr : nullptr;
			int width, height, nrChannels;
			if (stbi_is_hdr(name.c_str())) {
				float* data = stbi_loadf(name.c_str(), &width, &height, &nrChannels, 0);
				if (data == nullptr) return;
				if (ptr) {
					ptr->LoadTexture(width, height, nrChannels, data);
				}
				else next = new Texture(width, height, nrChannels, data);
				stbi_image_free(data);
			}
			else
			{
				uint8* data = stbi_load(name.c_str(), &width, &height, &nrChannels, 0);
				if (data == nullptr) return;
				int type = 0;
				if (name.rbegin()[4] == 'd' || name.rbegin()[5] == 'd') type = 0;
				else if (name.rbegin()[4] == 'n' || name.rbegin()[5] == 'n') type = 1;
				if (ptr) {
					ptr->LoadTexture(width, height, nrChannels, data, type);
				}
				else next = new Texture(width, height, nrChannels, data, type);
				stbi_image_free(data);
			}
			next->SetName(name);
			LoadedTextures.emplace(name, next);
			IRender::SendCommand({ RC_MAKEOBJECT, (uint64)static_cast<OpenGLObject*>(next), 0 });
			Console::Log("Texture loaded: " + name);
		}

		void Loader() {

			bool exit = false;
			while (!exit) {
				auto data = LoadQueue.dequeue();
				String& name = data.first;
				AssetType type = GetAssetType(name);
				switch (type)
				{
				case AssetType::MeshStatic: {
					auto mesh = loadMeshes(name);
					if (!mesh) continue;
					if (data.second) {
						auto container = (RenderMeshStaticGL*)data.second;
						container->SetMesh(mesh);
					}
					StaticMeshes.emplace(name, mesh);
					for (auto& m : mesh->Holders)
						IRender::SendCommand({ RC_MAKEOBJECT, (uint64)static_cast<OpenGLObject*>(m), 0 });
				} break;
				case AssetType::Texture: {
					TextureLoader(name, (Texture*)data.second);
				} break;
				default:
					break;
				}
			}
		}
	}
}

void AssetManager::StartLoader()
{
	LoaderThread = new std::thread(&Loader);
}

void AssetManager::LoadAssetAsync(const String& name)
{
	LoadQueue.enqueue({ name, nullptr });
}

void AssetManager::LoadMeshAsync(const String& name, RenderMeshStatic* empty)
{
	LoadQueue.enqueue({ name, empty });
}

void AssetManager::LoadTextureAsync(const String& name, Texture* empty)
{
	LoadQueue.enqueue({ name, empty });
}

AssetType AssetManager::GetAssetType(const String& name)
{
	// TODO
	if (name.find("Meshes") != String::npos) return AssetType::MeshStatic;
	if (name.find("Textures") != String::npos) return AssetType::Texture;
	if (name.find("Animations") != String::npos) return AssetType::MeshStatic;
	return AssetType::Texture;
}

RenderMeshStatic* AssetManager::LoadMeshAsyncWithPromise(const String& name)
{
	if (auto it = StaticMeshes.find(name); it != StaticMeshes.end()) {
		auto mesh = new RenderMeshStaticGL();
		mesh->SetMesh(it->second);
		return mesh;
	}
	else {
		auto mesh = MI->MakeEmptyStatic();
		LoadMeshAsync(name, mesh);
		return mesh;
	}
	return nullptr;
}

Texture* AssetManager::LoadTextureAsyncWithPromise(const String& name)
{
	if (auto it = LoadedTextures.find(name); it != LoadedTextures.end()) {
		return it->second;
	}
	else {
		auto tex = new Texture();
		LoadTextureAsync(name, tex);
		return tex;
	}
}
