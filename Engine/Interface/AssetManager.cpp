#include "AssetManager.h"
#include <RenderCore/RenderCommands.h>
#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Mesh.h>
#include <SkeletalMesh.h>
#include <Texture.h>
#include <Basic/QueueSafe.h>

#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define MAX_BONE_WEIGHTS 4

#define importOptions		\
aiProcess_Triangulate |		\
aiProcess_FlipUVs |			\
aiProcess_CalcTangentSpace | \
aiProcess_GenSmoothNormals

typedef std::vector<std::pair<String, Bone>> BoneHierarchy;

namespace AssetManager
{
	namespace {

		std::thread* LoaderThread;

		std::unordered_map<String, LoadedMesh*> StaticMeshes;
		std::unordered_map<String, LoadedSkeletalMesh*> SkeletalMeshes;
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
	MeshDataHolder* section = new MeshDataHolder(vertices.data(), (uint32)vertices.size(), indices.data(), (uint32)indices.size());
	section->Radius = radius;

	section->DefaultMaterial = IRender::LoadMaterialByName("Assets/Materials/default");

	meshHolder->HolderCount++;
	meshHolder->Holders.push_back(section);
}

void SetVertexBoneDataToDefault(SkinnedVertex& vertex)
{
	for (int i = 0; i < MAX_BONE_WEIGHTS; i++)
	{
		vertex.bones[i] = -1;
		vertex.weights[i] = 0.0f;
	}
}

void SetVertexBoneData(SkinnedVertex& vertex, int boneID, float weight)
{
	for (int i = 0; i < MAX_BONE_WEIGHTS; ++i)
	{
		if (vertex.bones[i] < 0)
		{
			vertex.weights[i] = weight;
			vertex.bones[i] = boneID;
			break;
		}
	}
}

void ExtractBoneWeightForVertices(std::vector<SkinnedVertex>& vertices, LoadedSkeletalMesh* holder, aiMesh* mesh)
{
	for (uint boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
	{
		int boneID = -1;
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
		for (int i = 0; i < holder->skeleton->BoneCount; i++) {
			if (holder->skeleton->Bones[i].name == boneName) {
				boneID = holder->skeleton->Bones[i].id;
				holder->skeleton->Bones[i].offset = glm::transpose(glm::make_mat4(&mesh->mBones[boneIndex]->mOffsetMatrix.a1));
				break;
			}
		}

		assert(boneID != -1);
		auto weights = mesh->mBones[boneIndex]->mWeights;
		int numWeights = mesh->mBones[boneIndex]->mNumWeights;

		for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
		{
			int vertexId = weights[weightIndex].mVertexId;
			float weight = weights[weightIndex].mWeight;
			assert(vertexId <= vertices.size());
			SetVertexBoneData(vertices[vertexId], boneID, weight);
		}
	}
}

void processSkinnedMesh(LoadedSkeletalMesh* meshHolder, aiMesh* mesh, BoneHierarchy& bh)
{
	std::vector<SkinnedVertex> vertices;
	std::vector<uint32> indices;
	float radius = 0.f;
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		SkinnedVertex vertex;
		SetVertexBoneDataToDefault(vertex);

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
	ExtractBoneWeightForVertices(vertices, meshHolder, mesh);

	SkeletalMeshDataHolder* section = new SkeletalMeshDataHolder(vertices.data(), (uint32)vertices.size(), indices.data(), (uint32)indices.size());
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

void processSkinnedNode(LoadedSkeletalMesh* mesh, BoneHierarchy& bh, aiNode* node, const aiScene* scene, bool usemesh)
{
	if (usemesh) {
		// process each mesh located at the current node
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			// the node object only contains indices to index the actual objects in the scene.
			aiMesh* meshed = scene->mMeshes[node->mMeshes[i]];
			processSkinnedMesh(mesh, meshed, bh);
		}
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processSkinnedNode(mesh, bh, node->mChildren[i], scene, usemesh);
		}
	}
	else {
		// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
		if (node->mParent != nullptr && mesh->skeleton && !mesh->skeleton->Bones && node->mNumMeshes == 0) {
			Bone& b = bh.emplace_back(node->mName.C_Str(), Bone()).second;
			for (int i = 0; i < bh.size(); i++) {
				if (bh[i].first == node->mParent->mName.C_Str()) {
					b.parent = i;
					break;
				}
			}
			b.name = node->mName.C_Str();
			b.base = glm::transpose(glm::make_mat4(&node->mTransformation.a1));
		}
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processSkinnedNode(mesh, bh, node->mChildren[i], scene, usemesh);
		}
	}
}

LoadedMesh* loadMeshes(const std::string& path)
{
	LoadedMesh* mesh = new LoadedMesh();
	//read file with Assimp
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path + ".obj", importOptions);
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

LoadedSkeletalMesh* loadMeshesSkinned(const std::string& path)
{
	LoadedSkeletalMesh* mesh = new LoadedSkeletalMesh();
	mesh->skeleton = new Skeleton();
	//read file with Assimp
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path + ".fbx", importOptions );
	//Check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		Console::Log("Error loading model file " + path + " : " + importer.GetErrorString());
		delete mesh;
		return nullptr;
	}
	// retrieve the directory path of the filepath
	std::string dir = path.substr(0, path.find_last_of('/'));
	BoneHierarchy bh;
	processSkinnedNode(mesh, bh, scene->mRootNode, scene, false);

	mesh->skeleton->Bones = new Bone[bh.size()]();
	mesh->skeleton->BoneCount = bh.size();
	Bone* bones = mesh->skeleton->Bones;
	for (int i = 0; i < bh.size(); i++) {
		bones[i] = bh[i].second;
		bones[i].id = i;
	}

	processSkinnedNode(mesh, bh, scene->mRootNode, scene, true);

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

				case AssetType::MeshSkeletal: {
					auto mesh = loadMeshesSkinned(name);
					if (!mesh) continue;
					if (data.second) {
						auto container = (RenderMeshSkeletalGL*)data.second;
						container->SetMesh(mesh);
					}
					SkeletalMeshes.emplace(name, mesh);
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

void AssetManager::LoadMeshAsync(const String& name, RenderMesh* empty)
{
	if (empty && empty->GetMeshType() == RenderMesh::MeshType::Static)
		LoadQueue.enqueue({ name, empty });
}

void AssetManager::LoadSkeletalMeshAsync(const String& name, RenderMesh* empty)
{
	if (empty && empty->GetMeshType() == RenderMesh::MeshType::Skeletal)
		LoadQueue.enqueue({ name, empty });
}

void AssetManager::LoadTextureAsync(const String& name, Texture* empty)
{
	LoadQueue.enqueue({ name, empty });
}

AssetType AssetManager::GetAssetType(const String& name)
{
	if (name.find("Meshes") != String::npos) {
		if (std::filesystem::exists(name + ".fbx")) return AssetType::MeshSkeletal;
		else return AssetType::MeshStatic;
	}
	if (name.find("Textures") != String::npos) return AssetType::Texture;
	if (name.find("Animations") != String::npos) return AssetType::Animation;
	return AssetType::None;
}

RenderMesh* AssetManager::LoadMeshAsyncWithPromise(const String& name)
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

RenderMesh* AssetManager::LoadSkeletalMeshAsyncWithPromise(const String& name)
{
	if (auto it = SkeletalMeshes.find(name); it != SkeletalMeshes.end()) {
		auto mesh = new RenderMeshSkeletalGL();
		mesh->SetMesh(it->second);
		return mesh;
	}
	else {
		auto mesh = MI->MakeEmptySkeletal();
		LoadSkeletalMeshAsync(name, mesh);
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
