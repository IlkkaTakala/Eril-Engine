#include "Mesh.h"
#include <GLFW/glfw3.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Objects/VisibleObject.h"

Section::Section()
{
	Instance = nullptr;
	Parent = nullptr;
	Holder = nullptr;
}

Section::~Section()
{
	if (Instance != nullptr) Instance->RemoveSection(this);
}

RenderObject::RenderObject(LoadedMesh* mesh)
{
	mesh->Users++;
	Mesh = mesh;

	SectionCount = mesh->HolderCount;
	Sections = new Section[SectionCount]();
	for (uint32 i = 0; i < mesh->HolderCount; i++) {
		Sections[i].Holder = mesh->Holders[i];
		SetMaterial(i, mesh->Holders[i]->Instance);
		Sections[i].Parent = this;
	}
}

RenderObject::~RenderObject()
{
	delete[] Sections;
	Mesh->Users--;
}

void RenderObject::SetMaterial(uint section, Material* nextMat)
{
	if (section < SectionCount && nextMat != nullptr) {
		if (Sections[section].Instance != nullptr) Sections[section].Instance->RemoveSection(&Sections[section]);
		Sections[section].Instance = nextMat;
		Sections[section].Instance->AddSection(&Sections[section]);
	}
}

const glm::mat4& RenderObject::GetModelMatrix()
{
	return ModelMatrix;
}

void RenderObject::SetParent(VisibleObject* parent)
{
	Parent = parent;
}

void RenderObject::ApplyTransform()
{
	Vector loc = Parent->GetLocation();
	Vector rot = Parent->GetRotation();
	Vector sca = Parent->GetScale();
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(loc.X, loc.Z, loc.Y))
		* glm::toMat4(glm::quat(glm::vec3(glm::radians(rot.X), glm::radians(rot.Z), glm::radians(rot.Y))))
		* glm::scale(glm::mat4(1.0f), glm::vec3(sca.X, sca.Z, sca.Y));
}

MeshDataHolder::MeshDataHolder()
{
	vertices = nullptr;
	indices = nullptr;
	VertexCount = 0;
	FaceCount = 0;
	Instance = nullptr;
}

MeshDataHolder::~MeshDataHolder()
{
	delete[] vertices;
	delete[] indices;
}

LoadedMesh::LoadedMesh()
{
	Users = 0;
	HolderCount = 0;
}

LoadedMesh::~LoadedMesh()
{
	for (auto const& sec : Holders)
		delete sec;
}
