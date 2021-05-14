#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <vector>
#include "Material.h"

class LoadedMesh;
class MeshDataHolder;
class Section;
class RenderObject;

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 uv;
};

class MeshDataHolder
{
public:
	MeshDataHolder();
	~MeshDataHolder();

	Material* Instance;
	Vertex* vertices;
	uint32* indices;
	uint32 VertexCount;
	uint32 FaceCount;
};


class LoadedMesh
{
public:
	LoadedMesh();
	~LoadedMesh();

	MeshDataHolder* Holders;
	uint32 HolderCount;
	uint32 Users;
};

class Section
{
public:
	Section();
	~Section();
	Material* Instance;
	RenderObject* Parent;

	const Vertex* GetVertices() const { return Holder->vertices; }
	const uint32* GetIndices() const { return Holder->indices; }
	const uint32 GetVertexCount() const { return Holder->VertexCount; }
	const uint32 GetFaceCount() const { return Holder->FaceCount; }


private:
	friend class RenderObject;

	MeshDataHolder* Holder;
};


class RenderObject : public RenderMesh
{
public:
	RenderObject(LoadedMesh* mesh);
	virtual ~RenderObject();

	void SetMaterial(uint section, Material* nextMat);
	Material* GetMaterial(uint section) const { if (section < SectionCount) return Sections[section].Instance; }
	const glm::mat4& GetModelMatrix();
	virtual void ApplyTransform() override;

	void SetParent(VisibleObject* parent);

private:

	VisibleObject* Parent;
	Section* Sections;
	LoadedMesh* Mesh;
	uint SectionCount;
	glm::mat4 ModelMatrix;
};