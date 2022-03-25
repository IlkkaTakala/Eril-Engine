#pragma once
#include <Interface/IRender.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <vector>
#include "Material.h"
#include "../OpenGLObject.h"

class LoadedMesh;
class MeshDataHolder;
class Section;
class RenderMeshStaticGL;

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 uv;
	glm::vec3 tangent;
};

class GLMesh : public IMesh
{
public:
	GLMesh() {}
	virtual ~GLMesh() {}
	virtual RenderMeshStatic* CreateProcedural(SceneComponent* parent, String name, std::vector<Vector>& positions, std::vector<Vector> UV, std::vector<Vector>& normal, std::vector<Vector>& tangent, std::vector<uint32>& indices) override;
	virtual RenderMeshStatic* GetStatic(SceneComponent* parent, const String& name) override;
	virtual RenderMeshStatic* MakeEmptyStatic() override;
};

class MeshDataHolder : public OpenGLObject
{
public:
	MeshDataHolder(Vertex* verts, uint32 vertCount, uint32* indices, uint32 indexCount);
	~MeshDataHolder();
	void CreateState() override;
	void Clear() override;

	uint VAO;
	uint VBO;
	uint EBO;
	uint defaultInstanced;

	std::vector<Vertex> temp_vertices;
	std::vector<uint32> temp_indices;
	Material* DefaultMaterial;
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
	RenderMeshStaticGL* Parent;
	float Radius;
	float RenderDistance;
	uint InstanceDisp;
	bool InstancesSet;

	void Render();
	float GetRadius() const { return Radius; }

	const uint32 GetVertexCount() const { return Holder->VertexCount; }
	const uint32 GetFaceCount() const { return Holder->IndexCount / 3; }

private:
	friend class RenderMeshStaticGL;

	MeshDataHolder* Holder;
};


class RenderMeshStaticGL : public RenderMeshStatic
{
public:
	RenderMeshStaticGL();
	virtual ~RenderMeshStaticGL();

	void SetMesh(LoadedMesh* mesh);

	virtual void SetMaterial(uint section, Material* nextMat) override;
	virtual Material* GetMaterial(uint section) const override { if (section < SectionCount) return Sections[section].Instance; else if (auto it = Materials.find(section); it != Materials.end()) return it->second; else return nullptr; }
	const glm::mat4& GetModelMatrix();
	virtual void ApplyTransform() override;
	virtual void SetInstances(int count, Transformation* dispArray) override;
	virtual void SetInstanceCount(int count) override;

	void ApplyInstances();

	virtual void SetSectionRenderDistance(uint section, float distance) override;

	virtual SceneComponent* GetParent() const { return Parent; }
	virtual void SetParent(SceneComponent* p) override { Parent = p; }

	virtual void SetBinds(std::function<void(void)> bind) override;
	virtual std::function<void(void)>& GetBinds() override;

	virtual void SetAABB(AABB bounds) override;

private:
	friend class Renderer;
	friend class Section;

	SceneComponent* Parent;
	Section* Sections;
	LoadedMesh* Mesh;
	uint SectionCount;
	glm::mat4 ModelMatrix;
	bool requireUpdate;

	int InstanceCount;
	int InstanceCountMax;
	glm::mat4* Instances;
	bool InstancesDirty;
	bool Instanced;

	std::unordered_map<uint, Material*> Materials;

	std::function<void(void)> binds;
};