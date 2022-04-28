#pragma once
#include <Interface/IRender.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <vector>
#include <Mesh.h>
#include "Material.h"
#include "Skeleton.h"

class LoadedSkeletalMesh;
class SkeletalMeshDataHolder;
class SkinnedSection;
class RenderMeshSkeletalGL;

struct SkinnedVertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 uv;
	glm::vec3 tangent;

	glm::vec4 weights;
	glm::ivec4 bones;
};

class SkeletalMeshDataHolder : public OpenGLObject
{
public:
	SkeletalMeshDataHolder(SkinnedVertex* verts, uint32 vertCount, uint32* indices, uint32 indexCount);
	~SkeletalMeshDataHolder();
	void CreateState() override;
	void Clear() override;

	uint VAO;
	uint VBO;
	uint EBO;
	uint defaultInstanced;

	std::vector<SkinnedVertex> temp_vertices;
	std::vector<uint32> temp_indices;
	Material* DefaultMaterial;
	uint32 VertexCount;
	uint32 IndexCount;
	float Radius;
};


class LoadedSkeletalMesh
{
public:
	LoadedSkeletalMesh();
	~LoadedSkeletalMesh();

	Skeleton* skeleton;
	std::vector<SkeletalMeshDataHolder*> Holders;
	uint32 HolderCount;
	uint32 Users;
	uint Time;
};

class SkinnedSection : public Section
{
public:
	SkinnedSection();
	virtual ~SkinnedSection();
	RenderMeshSkeletalGL* mesh;

	void Render() override;

	const uint32 GetVertexCount() const { return Holder->VertexCount; }
	const uint32 GetFaceCount() const { return Holder->IndexCount / 3; }

private:
	friend class RenderMeshSkeletalGL;

	SkeletalMeshDataHolder* Holder;
};

class RenderMeshSkeletalGL : public RenderMeshGL
{
public:
	RenderMeshSkeletalGL();
	virtual ~RenderMeshSkeletalGL();

	void SetMesh(LoadedSkeletalMesh* mesh);

	void CreateState() override;
	void Clear() override;

	virtual void SetMaterial(uint section, Material* nextMat) override;
	virtual Material* GetMaterial(uint section) const override { if (section < SectionCount) return Sections[section].Instance; else if (auto it = Materials.find(section); it != Materials.end()) return it->second; else return nullptr; }
	virtual void ApplyTransform(float delta) override;

	virtual void SetSectionRenderDistance(uint section, float distance) override;

	virtual void SetAABB(AABB bounds) override;

	void UpdateBoneMatrices();
	std::vector<glm::mat4>& GetBoneMatrices() { return BoneTransformArray; }

private:
	friend class Renderer;
	friend class SkinnedSection;

	SkinnedSection* Sections;
	LoadedSkeletalMesh* Mesh;
	uint SectionCount;
	uint BoneTranforms;
	bool requireUpdate;
	std::vector<glm::mat4> BoneTransformArray;

	std::unordered_map<uint, Material*> Materials;
};