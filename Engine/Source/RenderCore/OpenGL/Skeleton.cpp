#include "Skeleton.h"

Skeleton::Skeleton()
{
	SkeletonID = 0;
	BoneCount = 0;
	Bones = nullptr;
}

Skeleton::~Skeleton()
{
	delete[] Bones;
}
