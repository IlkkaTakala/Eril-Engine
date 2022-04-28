#include "SkeletalMesh.h"
#include <GLFW/glfw3.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Objects/VisibleObject.h"
#include <glm/gtx/euler_angles.hpp>
#include <Interface/AssetManager.h>
#include <RenderCore/RenderCommands.h>

SkinnedSection::SkinnedSection()
{
	Instance = nullptr;
	Parent = nullptr;
	Holder = nullptr;
	RenderDistance = 100000.f;
	Radius = 0.f;
}

SkinnedSection::~SkinnedSection()
{
}

void SkinnedSection::Render()
{
	if (Holder->VAO == 0 || !Parent->IsVisible()) return;

	glBindVertexArray(Holder->VAO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, mesh->BoneTranforms);

	if (mesh && mesh->GetBinds()) mesh->GetBinds()();

	glDrawElements(GL_TRIANGLES, Holder->IndexCount, GL_UNSIGNED_INT, 0);
}

void RenderMeshSkeletalGL::SetMesh(LoadedSkeletalMesh* mesh)
{
	mesh->Users++;
	mesh->Time = 0;
	Mesh = mesh;
	float extent = 0.f;

	SectionCount = mesh->HolderCount;
	Sections = new SkinnedSection[SectionCount]();
	for (uint32 i = 0; i < mesh->HolderCount; i++) {
		Sections[i].Holder = mesh->Holders[i];
		Sections[i].Radius = mesh->Holders[i]->Radius;

		if (auto it = Materials.find(i); it != Materials.end())
			SetMaterial(i, it->second);
		else SetMaterial(i, mesh->Holders[i]->DefaultMaterial);
		Sections[i].Parent = this;
		Sections[i].mesh = this;

		if (Sections[i].GetRadius() > extent) extent = Sections[i].GetRadius();
	}

	IRender::SendCommand({RC_MAKEOBJECT, (uint64)static_cast<OpenGLObject*>(this), 0 });

	bounds = extent;
	requireUpdate = true;
}

void RenderMeshSkeletalGL::CreateState()
{
	glGenBuffers(1, &BoneTranforms);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, BoneTranforms);
	BoneTransformArray.resize(Mesh->skeleton->BoneCount, glm::mat4(1.f));
	for (uint i = 0; i < Mesh->skeleton->BoneCount; i++) {
		const Bone& b = Mesh->skeleton->Bones[i];
		int p = b.parent;
		if (p < 0 || p > i) continue;
		BoneTransformArray[i] = BoneTransformArray[p] * b.base;
	}
	for (uint i = 0; i < Mesh->skeleton->BoneCount; i++) {
		const Bone& b = Mesh->skeleton->Bones[i];
		BoneTransformArray[i] *= b.offset;
	}
	glBufferData(GL_SHADER_STORAGE_BUFFER, Mesh->skeleton->BoneCount * sizeof(glm::mat4), BoneTransformArray.data(), GL_STATIC_DRAW);
	requireUpdate = false;
	SetVisible(true);
}

void RenderMeshSkeletalGL::Clear()
{
	glDeleteBuffers(1, &BoneTranforms);
}

RenderMeshSkeletalGL::RenderMeshSkeletalGL() : RenderMeshGL()
{
	Parent = nullptr;
	Sections = nullptr;
	Mesh = nullptr;
	SectionCount = 0;
	requireUpdate = true;
	type = MeshType::Skeletal;
	BoneTranforms = 0;
}

RenderMeshSkeletalGL::~RenderMeshSkeletalGL()
{
	delete[] Sections;
	if (Mesh) Mesh->Users--;
	Clear(); // TODO
}

void RenderMeshSkeletalGL::SetMaterial(uint section, Material* nextMat)
{
	Materials[section] = nextMat;
	if (section < SectionCount && nextMat != nullptr) {
		if (Sections[section].Instance != nullptr) Sections[section].Instance->RemoveSection(&Sections[section]);
		Sections[section].Instance = nextMat;
		Sections[section].Instance->AddSection(&Sections[section]);
	}
}

void RenderMeshSkeletalGL::SetAABB(AABB bounds)
{
	RenderMesh::SetAABB(bounds);

	float rad = glm::max(glm::max(bounds.maxs.X, bounds.maxs.Y), bounds.maxs.Z);
	for (uint i = 0; i < SectionCount; i++) {
		Sections[i].Radius = rad;
	}
}

void RenderMeshSkeletalGL::UpdateBoneMatrices()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, BoneTranforms);
	for (uint i = 0; i < Mesh->skeleton->BoneCount; i++) {
		const Bone& b = Mesh->skeleton->Bones[i];
		BoneTransformArray[i] *= b.offset;
	}
	glBufferData(GL_SHADER_STORAGE_BUFFER, Mesh->skeleton->BoneCount * sizeof(glm::mat4), BoneTransformArray.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void RenderMeshSkeletalGL::ApplyTransform(float delta)
{
	Transformation finalT;
	SceneComponent* parent = Parent;
	while (parent)
	{
		finalT += parent->GetTransformation();

		if (parent->GetParent() == nullptr) break;

		parent = parent->GetParent();
	}

	Vector loc = finalT.Location;
	Rotator rot = finalT.Rotation;
	Vector sca = finalT.Scale;
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(loc.X, loc.Y, loc.Z))
		* glm::toMat4(glm::quat(rot.W, rot.X, rot.Y, rot.Z))
		* glm::scale(glm::mat4(1.0f), glm::vec3(sca.X, sca.Y, sca.Z));


	requireUpdate = false;

	for (auto& [id, func] : renderCallbacks) {
		func(delta);
	}
}

void RenderMeshSkeletalGL::SetSectionRenderDistance(uint section, float distance)
{
	if (section < SectionCount) {
		Sections[section].RenderDistance = distance;
	}
}

SkeletalMeshDataHolder::SkeletalMeshDataHolder(SkinnedVertex* verts, uint32 vertCount, uint32* indices, uint32 indexCount)
{
	VAO = 0;
	VBO = 0;
	EBO = 0;
	temp_vertices.resize(vertCount);
	temp_indices.resize(indexCount);
	std::copy(verts, verts + vertCount, temp_vertices.begin());
	std::copy(indices, indices + indexCount, temp_indices.begin());
	DefaultMaterial = nullptr;
	Radius = 0.f;
	defaultInstanced = 0;
}

SkeletalMeshDataHolder::~SkeletalMeshDataHolder()
{
	Clear();
}

void SkeletalMeshDataHolder::CreateState()
{
	VertexCount = temp_vertices.size();
	IndexCount = temp_indices.size();

	glGenVertexArrays(1, &VAO);

	// Create Vertex Buffer Object
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then 
	// configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexCount * sizeof(uint32), temp_indices.data(), GL_STATIC_DRAW);

	// Set buffer data to m_vbo-object (bind buffer first and then set the data), dynamic buffer with max size calculated from maximum batch size
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, VertexCount * sizeof(SkinnedVertex), temp_vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, uv));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, normal));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, tangent));
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, weights));
	glEnableVertexAttribArray(4);

	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, weights));
	glEnableVertexAttribArray(4);
		
	glVertexAttribIPointer(5, 4, GL_INT, sizeof(SkinnedVertex), (void*)offsetof(SkinnedVertex, bones));
	glEnableVertexAttribArray(5);

	glBindVertexArray(0);

	temp_indices.clear();
	temp_vertices.clear();
}

void SkeletalMeshDataHolder::Clear()
{
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
}

LoadedSkeletalMesh::LoadedSkeletalMesh()
{
	Users = 0;
	HolderCount = 0;
	Time = 0;
}

LoadedSkeletalMesh::~LoadedSkeletalMesh()
{
	for (auto const& sec : Holders)
		delete sec;
	delete skeleton;
}
