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
	glm::vec3 tangent;
};

class MeshDataHolder
{
public:
	MeshDataHolder(Vertex* verts, uint32 vertCount, uint32* indices, uint32 indexCount);
	~MeshDataHolder();

	uint VAO;
	uint VBO;
	uint EBO;

	Material* Instance;
	uint32 VertexCount;
	uint32 IndexCount;
	float Radius;
};


class LoadedMesh
{
public:
	LoadedMesh();
	~LoadedMesh();

	std::vector<MeshDataHolder*> Holders;
	uint32 HolderCount;
	uint32 Users;
	uint Time;
};

class Section
{
public:
	Section();
	~Section();
	Material* Instance;
	RenderObject* Parent;
	bool Instanced;
	int InstanceCount;
	int InstanceCountMax;
	uint InstanceDisp;
	float Radius;

	void Render();
	void MakeInstanced(int count, const glm::mat4* modelM);
	float GetRadius() const { return Radius; }

	const uint32 GetVertexCount() const { return Holder->VertexCount; }
	const uint32 GetFaceCount() const { return Holder->IndexCount / 3; }

private:
	friend class RenderObject;

	MeshDataHolder* Holder;
};


class RenderObject : public RenderMesh
{
public:
	RenderObject(LoadedMesh* mesh);
	virtual ~RenderObject();

	virtual void SetMaterial(uint section, Material* nextMat) override;
	virtual Material* GetMaterial(uint section) const override { if (section < SectionCount) return Sections[section].Instance; else return nullptr; }
	const glm::mat4& GetModelMatrix();
	virtual void ApplyTransform() override;
	virtual void SetInstances(int count, Transformation* dispArray) override;
	virtual void SetInstanceCount(int count) override;

	void SetParent(SceneComponent* parent);
	virtual SceneComponent* GetParent() const { return Parent; }

	virtual void SetBinds(std::function<void(void)> bind) override;
	virtual std::function<void(void)>& GetBinds() override;

	virtual void SetAABB(AABB bounds) override;

private:
	friend class Renderer;
	SceneComponent* Parent;
	Section* Sections;
	LoadedMesh* Mesh;
	uint SectionCount;
	glm::mat4 ModelMatrix;
	bool requireUpdate;

	std::function<void(void)> binds;
};