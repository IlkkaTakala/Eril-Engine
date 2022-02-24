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
	Instanced = false;
	InstanceCount = 1;
	InstanceDisp = 0;
}

Section::~Section()
{
	if (Instance != nullptr) Instance->RemoveSection(this);
	glDeleteBuffers(1, &InstanceDisp);
}

void Section::Render()
{
	glBindVertexArray(Holder->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, InstanceDisp);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(0));
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 4));
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 8));
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 12));

	if (Instanced) glDrawElementsInstanced(GL_TRIANGLES, Holder->IndexCount, GL_UNSIGNED_INT, 0, InstanceCount);
	else glDrawElements(GL_TRIANGLES, Holder->IndexCount, GL_UNSIGNED_INT, 0);
}

void Section::MakeInstanced(int count, const glm::mat4* modelM)
{
	if (InstanceDisp != 0) glDeleteBuffers(1, &InstanceDisp);
	InstanceCount = count;
	Instanced = true;

	glBindVertexArray(Holder->VAO);

	glGenBuffers(1, &InstanceDisp);
	glBindBuffer(GL_ARRAY_BUFFER, InstanceDisp);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * count, modelM, GL_STATIC_DRAW);

	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);
	glEnableVertexAttribArray(7);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(0));
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 4));
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 8));
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 12));
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	glVertexAttribDivisor(7, 1);
	
	glBindVertexArray(0);

}

RenderObject::RenderObject(LoadedMesh* mesh)
{
	mesh->Users++;
	mesh->Time = 0;
	Mesh = mesh;
	float extent = 0.f;

	SectionCount = mesh->HolderCount;
	Sections = new Section[SectionCount]();
	for (uint32 i = 0; i < mesh->HolderCount; i++) {
		Sections[i].Holder = mesh->Holders[i];

		glm::mat4 trans(1.f);

		glBindVertexArray(Sections[i].Holder->VAO);

		glGenBuffers(1, &Sections[i].InstanceDisp);
		glBindBuffer(GL_ARRAY_BUFFER, Sections[i].InstanceDisp);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), &trans, GL_STATIC_DRAW);

		glEnableVertexAttribArray(4);
		glEnableVertexAttribArray(5);
		glEnableVertexAttribArray(6);
		glEnableVertexAttribArray(7);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(0));
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 4));
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 8));
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 12));
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);

		glBindVertexArray(0);
		SetMaterial(i, mesh->Holders[i]->Instance);
		Sections[i].Parent = this;

		if (Sections[i].GetRadius() > extent) extent = Sections[i].GetRadius();
	}
	bounds = extent;
	requireUpdate = true;
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
	requireUpdate = true;
	return ModelMatrix;
}

void RenderObject::SetParent(VisibleObject* parent)
{
	Parent = parent;
}

void RenderObject::ApplyTransform()
{
	Transformation finalT;
	SceneComponent* parent = Parent;
	while (true)
	{
		finalT += parent->GetTransformation();

		if (parent->GetParent() == nullptr) break;

		parent = parent->GetParent();
	}

	Vector loc = finalT.Location;
	Vector rot = finalT.Rotation;
	Vector sca = finalT.Scale;
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(loc.X, loc.Z, loc.Y))
		* glm::toMat4(glm::quat(glm::vec3(glm::radians(rot.X), glm::radians(rot.Z), glm::radians(rot.Y))))
		* glm::scale(glm::mat4(1.0f), glm::vec3(sca.X, sca.Z, sca.Y));

	requireUpdate = false;
}

void RenderObject::SetInstances(int count, Transformation* dispArray)
{
	glm::mat4* arr = new glm::mat4[count]();
	for (int i = 0; i < count; i++) {
		Vector loc = dispArray[i].Location;
		Vector rot = dispArray[i].Rotation;
		Vector sca = dispArray[i].Scale;
		arr[i] = glm::translate(glm::mat4(1.0f), glm::vec3(loc.X, loc.Z, loc.Y))
			* glm::toMat4(glm::quat(glm::vec3(glm::radians(rot.X), glm::radians(rot.Z), glm::radians(rot.Y))))
			* glm::scale(glm::mat4(1.0f), glm::vec3(sca.X, sca.Z, sca.Y));
	}
	for (uint i = 0; i < SectionCount; i++) {
		Sections[i].MakeInstanced(count, arr);
	}
}

MeshDataHolder::MeshDataHolder(Vertex* verts, uint32 vertCount, uint32* indices, uint32 indexCount)
{
	VertexCount = vertCount;
	IndexCount = indexCount;
	Instance = nullptr;
	Radius = 0.f;

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
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
}

LoadedMesh::LoadedMesh()
{
	Users = 0;
	HolderCount = 0;
	Time = 0;
}

LoadedMesh::~LoadedMesh()
{
	for (auto const& sec : Holders)
		delete sec;
}
