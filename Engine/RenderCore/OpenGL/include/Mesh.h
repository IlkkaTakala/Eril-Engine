#pragma once
#include <Interface/IRender.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <vector>
#include "Material.h"
#include "../OpenGLObject.h"
#include "AnimationController.h"
#include "Skeleton.h"

class LoadedMesh;
class MeshDataHolder;
class Section;
class RenderMeshGL;

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 uv;
	glm::vec3 tangent;
};

struct SkeletalVertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 uv;
	glm::vec3 tangent;

	glm::vec4 weights;
	glm::ivec4 bones;
};

class GLMesh : public IMesh
{
public:
	GLMesh() {}
	virtual ~GLMesh() {}
	virtual RenderMesh* CreateProcedural(SceneComponent* parent, String name, std::vector<Vector>& positions, std::vector<Vector> UV, std::vector<Vector>& normal, std::vector<Vector>& tangent, std::vector<uint32>& indices) override;
	virtual RenderMesh* GetStatic(SceneComponent* parent, const String& name) override;
	virtual RenderMesh* GetSkeletal(SceneComponent* parent, const String& name) override;
	virtual RenderMesh* MakeEmptyStatic() override;
	virtual RenderMesh* MakeEmptySkeletal() override;
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
	virtual ~Section();
	RenderMeshGL* Parent;
	Material* Instance;
	float Radius;
	float RenderDistance;

	virtual void Render() = 0;
	float GetRadius() const { return Radius; }

	virtual const uint32 GetVertexCount() const = 0;
	virtual const uint32 GetFaceCount() const = 0;
};

class StaticSection : public Section
{
public:
	StaticSection();
	virtual ~StaticSection();
	uint InstanceDisp;
	bool InstancesSet;
	RenderMeshStaticGL* mesh;

	void Render() override;

	const uint32 GetVertexCount() const { return Holder->VertexCount; }
	const uint32 GetFaceCount() const { return Holder->IndexCount / 3; }
private:
	friend class RenderMeshStaticGL;

	MeshDataHolder* Holder;
};

class RenderMeshGL : public RenderMesh, public OpenGLObject
{
public:
	virtual ~RenderMeshGL() {};
	virtual void ApplyTransform(float delta) = 0;
	virtual void CreateState() {};
	virtual void RefreshState() {};
	virtual void Clear() {};

	const glm::mat4& GetModelMatrix() const { return ModelMatrix; }

protected:
	RenderMeshGL() : RenderMesh() { ModelMatrix = glm::mat4(1.f); }
	
	glm::mat4 ModelMatrix;
private:
	RenderMeshGL(const RenderMeshGL&) = delete;
	RenderMeshGL& operator=(const RenderMeshGL&) = delete;
};

class RenderMeshStaticGL : public RenderMeshGL
{
public:
	RenderMeshStaticGL();
	virtual ~RenderMeshStaticGL();

	void SetMesh(LoadedMesh* mesh);

	virtual void SetMaterial(uint section, Material* nextMat) override;
	virtual Material* GetMaterial(uint section) const override { if (section < SectionCount) return Sections[section].Instance; else if (auto it = Materials.find(section); it != Materials.end()) return it->second; else return nullptr; }
	virtual void ApplyTransform(float delta) override;
	virtual void SetInstances(int count, Transformation* dispArray) override;
	virtual void SetInstanceCount(int count) override;

	void ApplyInstances();

	virtual void SetSectionRenderDistance(uint section, float distance) override;

	virtual void SetAABB(AABB bounds) override;

private:
	friend class Renderer;
	friend class StaticSection;

	StaticSection* Sections;
	LoadedMesh* Mesh;
	uint SectionCount;
	bool requireUpdate;

	int InstanceCount;
	int InstanceCountMax;
	glm::mat4* Instances;
	bool InstancesDirty;
	bool Instanced;

	std::unordered_map<uint, Material*> Materials;
};