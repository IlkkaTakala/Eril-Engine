#pragma once
#include <Core.h>
#include <glm/glm.hpp>

struct Bone
{
	Bone() {
		id = -1;
		parent = -1;
		base = glm::mat4(1.f);
		offset = glm::mat4(1.f);
		name = "";
	}

	int id;
	int parent;
	glm::mat4 base;
	glm::mat4 offset;
	String name;
};


class Skeleton
{
public:
	Skeleton();
	~Skeleton();

	uint SkeletonID;
	uint BoneCount;
	Bone* Bones;
};

