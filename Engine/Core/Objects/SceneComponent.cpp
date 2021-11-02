#include "VisibleObject.h"
#include "Physics.h"

SceneComponent::SceneComponent() : BaseObject()
{
	Location = Vector(0, 0, 0);
	Rotation = Vector(0, 0, 0);
	Scale = Vector(1, 1, 1);

	Parent = nullptr;
}

void SceneComponent::SetLocation(const Vector& NewLocation)
{
	Location = NewLocation;
}

void SceneComponent::SetRotation(const Vector& NewRotation)
{
	Rotation = NewRotation;
}

void SceneComponent::SetScale(const Vector& NewScale)
{
	Scale = NewScale;
}