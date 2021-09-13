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

void Section::Render()
{
	glBindVertexArray(Holder->VAO);
	glDrawElements(GL_TRIANGLES, Holder->FaceCount * 3, GL_UNSIGNED_INT, 0);
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

MeshDataHolder::MeshDataHolder(Vertex* verts, uint32 vertCount, uint32* indices, uint32 indexCount)
{
	VertexCount = vertCount;
	FaceCount = indexCount / 3;
	Instance = nullptr;

	glGenVertexArrays(1, &VAO);

	// Create Vertex Buffer Object
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then 
	// configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(uint32), indices, GL_STATIC_DRAW);

	// Set buffer data to m_vbo-object (bind buffer first and then set the data), dynamic buffer with max size calculated from maximum batch size
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, VertexCount * sizeof(Vertex), verts, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);
}

MeshDataHolder::~MeshDataHolder()
{
	//delete[] vertices;
	//delete[] indices;

	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
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
