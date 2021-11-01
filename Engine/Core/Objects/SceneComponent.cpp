#include "VisibleObject.h"
#include "Physics.h"

SceneComponent::SceneComponent() : BaseObject()
{
	Location = Vector(0, 0, 0);
	Rotation = Vector(0, 0, 0);
	Scale = Vector(1, 1, 1);
}

void SceneComponent::SetLocation(Vector NewLocation)
{
	Location = NewLocation;
}

void SceneComponent::SetRotation(Vector NewRotation)
{
	Rotation = NewRotation;
}

void SceneComponent::SetScale(Vector NewScale)
{
	Scale = NewScale;
}