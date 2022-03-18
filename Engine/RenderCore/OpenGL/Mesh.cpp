#include "Mesh.h"
#include <GLFW/glfw3.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Objects/VisibleObject.h"
#include <glm/gtx/euler_angles.hpp>
#include <Interface/AssetManager.h>
#include <RenderCore/RenderCommands.h>

RenderMeshStatic* GLMesh::GetStatic(SceneComponent* parent, const String& name)
{
	auto mesh = AssetManager::LoadMeshAsyncWithPromise(name);
	mesh->SetParent(parent);
	return mesh;
}

RenderMeshStatic* GLMesh::MakeEmptyStatic()
{
	return new RenderMeshStaticGL();
}

RenderMeshStatic* GLMesh::CreateProcedural(SceneComponent* parent, String name, std::vector<Vector>& positions, std::vector<Vector> UV, std::vector<Vector>& normal, std::vector<Vector>& tangent, std::vector<uint32>& indices)
{
	/*LoadedMesh* mesh = new LoadedMesh();

	std::vector<Vertex> verts;
	verts.resize(positions.size());

	AABB bounds;
	for (auto i = 0; i < positions.size(); i++) {
		if (bounds.mins.X > positions[i].X) bounds.mins.X = positions[i].X;
		else if (bounds.maxs.X < positions[i].X) bounds.maxs.X = positions[i].X;
		if (bounds.mins.Y > positions[i].Y) bounds.mins.Y = positions[i].Y;
		else if (bounds.maxs.Y < positions[i].Y) bounds.maxs.Y = positions[i].Y;
		if (bounds.mins.Z > positions[i].Z) bounds.mins.Z = positions[i].Z;
		else if (bounds.maxs.Z < positions[i].Z) bounds.maxs.Z = positions[i].Z;

		verts[i].position = glm::vec3(positions[i].X, positions[i].Z, positions[i].Y);
		verts[i].uv = glm::vec3(UV[i].X, UV[i].Y, UV[i].Z);
		verts[i].normal = glm::vec3(normal[i].X, normal[i].Z, normal[i].Y);
		verts[i].tangent = glm::vec3(tangent[i].X, tangent[i].Z, tangent[i].Y);
	}

	MeshDataHolder* section = new MeshDataHolder(verts.data(), positions.size(), indices.data(), indices.size());

	mesh->HolderCount++;
	mesh->Holders.push_back(section);
	section->Radius = bounds.maxs.Length();

	LoadedMeshes.emplace(name, mesh);
	RenderMeshStaticGL* obj = new RenderMeshStaticGL();
	obj->SetMesh(mesh);
	obj->SetParent(parent);
	obj->SetAABB(bounds);
	return obj;*/
	return nullptr;
}


Section::Section()
{
	Instance = nullptr;
	Parent = nullptr;
	Holder = nullptr;
	Instanced = false;
	InstanceCount = 1;
	InstanceCountMax = 1;
	InstanceDisp = 0;
	RenderDistance = 100000.f;
	Radius = 0.f;
	InstancesDirty = false;
	Instances = nullptr;
}

Section::~Section()
{
	if (Instance != nullptr) Instance->RemoveSection(this);
	glDeleteBuffers(1, &InstanceDisp); // TODO
	delete[] Instances;
}

void Section::Render()
{
	glBindVertexArray(Holder->VAO);

	if (Parent->GetBinds()) Parent->GetBinds()();

	if (Instanced) {
		if (InstancesDirty) {
			glGenBuffers(1, &InstanceDisp);
			glBindBuffer(GL_ARRAY_BUFFER, InstanceDisp);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * InstanceCount, Instances, GL_DYNAMIC_DRAW);

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
		} 
		else {
			glBindBuffer(GL_ARRAY_BUFFER, InstanceDisp);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(0));
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 4));
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 8));
			glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 12));
		}
		glDrawElementsInstanced(GL_TRIANGLES, Holder->IndexCount, GL_UNSIGNED_INT, 0, InstanceCount);
	}
	else glDrawElements(GL_TRIANGLES, Holder->IndexCount, GL_UNSIGNED_INT, 0);
}

void Section::MakeInstanced(int count, const glm::mat4* modelM)
{
	if (Instances != nullptr)
	{
		if (count < InstanceCountMax) {
			InstanceCount = count;
			memcpy(Instances, modelM, count);
		}
		else {
			InstanceCount = count;
			InstanceCountMax = count;
			Instanced = true;

			delete[] Instances;
			Instances = new glm::mat4[count]();
			memcpy(Instances, modelM, count);
		}
	}
	else {
		InstanceCount = count;
		InstanceCountMax = count;
		Instanced = true;
		
		Instances = new glm::mat4[count]();
		memcpy(Instances, modelM, count);
	
	}
	InstancesDirty = true;
}

void RenderMeshStaticGL::SetMesh(LoadedMesh* mesh)
{
	mesh->Users++;
	mesh->Time = 0;
	Mesh = mesh;
	float extent = 0.f;

	SectionCount = mesh->HolderCount;
	Sections = new Section[SectionCount]();
	for (uint32 i = 0; i < mesh->HolderCount; i++) {
		Sections[i].Holder = mesh->Holders[i];
		Sections[i].Radius = mesh->Holders[i]->Radius;

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
		SetMaterial(i, mesh->Holders[i]->DefaultMaterial);
		Sections[i].Parent = this;

		if (Sections[i].GetRadius() > extent) extent = Sections[i].GetRadius();
	}
	bounds = extent;
	requireUpdate = true;
}

RenderMeshStaticGL::RenderMeshStaticGL()
{
	Parent = nullptr;
	Sections = nullptr;
	Mesh = nullptr;
	SectionCount = 0;
	requireUpdate = true;
	ModelMatrix = glm::mat4(1.f);
}

RenderMeshStaticGL::~RenderMeshStaticGL()
{
	delete[] Sections;
	Mesh->Users--;
}

void RenderMeshStaticGL::SetMaterial(uint section, Material* nextMat)
{
	if (section < SectionCount && nextMat != nullptr) {
		if (Sections[section].Instance != nullptr) Sections[section].Instance->RemoveSection(&Sections[section]);
		Sections[section].Instance = nextMat;
		Sections[section].Instance->AddSection(&Sections[section]);
	}
}

const glm::mat4& RenderMeshStaticGL::GetModelMatrix()
{
	requireUpdate = true;
	return ModelMatrix;
}

void RenderMeshStaticGL::SetBinds(std::function<void(void)> bind)
{
	binds = bind;
}

std::function<void(void)>& RenderMeshStaticGL::GetBinds()
{
	return binds;
}

void RenderMeshStaticGL::SetAABB(AABB bounds)
{
	RenderMeshStatic::SetAABB(bounds);

	float rad = glm::max(glm::max(bounds.maxs.X, bounds.maxs.Y), bounds.maxs.Z);
	for (uint i = 0; i < SectionCount; i++) {
		Sections[i].Radius = rad;
	}
}

void RenderMeshStaticGL::ApplyTransform()
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
	Vector rot = finalT.Rotation;
	Vector sca = finalT.Scale;
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(loc.X, loc.Z, loc.Y))
		* glm::eulerAngleYXZ(glm::radians(rot.Z), glm::radians(rot.Y), glm::radians(rot.X))
		* glm::scale(glm::mat4(1.0f), glm::vec3(sca.X, sca.Z, sca.Y));

	requireUpdate = false;
}

void RenderMeshStaticGL::SetInstances(int count, Transformation* dispArray)
{
	glm::mat4* arr = new glm::mat4[count]();
	for (int i = 0; i < count; i++) {
		Vector loc = dispArray[i].Location;
		Vector rot = dispArray[i].Rotation;
		Vector sca = dispArray[i].Scale;
		arr[i] = glm::translate(glm::mat4(1.0f), glm::vec3(loc.X, loc.Z, loc.Y))
			* glm::eulerAngleYXZ(glm::radians(rot.Z), glm::radians(rot.Y), glm::radians(rot.X))
			* glm::scale(glm::mat4(1.0f), glm::vec3(sca.X, sca.Z, sca.Y));
	}
	for (uint i = 0; i < SectionCount; i++) {
		Sections[i].MakeInstanced(count, arr);
	}
	delete[] arr;
}

void RenderMeshStaticGL::SetInstanceCount(int count)
{
	for (uint i = 0; i < SectionCount; i++) {
		Sections[i].InstanceCount = count;
	}
}

void RenderMeshStaticGL::SetSectionRenderDistance(uint section, float distance)
{
	if (section < SectionCount) {
		Sections[section].RenderDistance = distance;
	}
}

MeshDataHolder::MeshDataHolder(Vertex* verts, uint32 vertCount, uint32* indices, uint32 indexCount)
{
	VAO = 0;
	VBO = 0;
	EBO = 0;
	temp_vertices.resize(vertCount);
	temp_indices.resize(indexCount);
	memcpy(temp_vertices.data(), verts, vertCount);
	memcpy(temp_indices.data(), indices, indexCount);
	DefaultMaterial = nullptr;
	Radius = 0.f;
}

MeshDataHolder::~MeshDataHolder()
{
	IRender::SendCommand({ RC_DELETEBUFFER, VBO, 0 });
	IRender::SendCommand({ RC_DELETEBUFFER, EBO, 0 });
	IRender::SendCommand({ RC_DELETEARRAY, VAO, 0 });
}

void MeshDataHolder::MakeBuffers()
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
	glBufferData(GL_ARRAY_BUFFER, VertexCount * sizeof(Vertex), temp_vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);

	temp_indices.clear();
	temp_vertices.clear();
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
