#include "VisibleObject.h"

VisibleObject::VisibleObject() : BaseObject()
{
	Location = Vector(0, 0, 0);
	Rotation = Vector(0, 0, 0);
	Scale = Vector(1, 1, 1);
	RenderData = nullptr;
}

void VisibleObject::SetLocation(Vector NewLocation)
{
	Location = NewLocation;
	if (RenderData != nullptr) RenderData->ApplyTransform();
}

void VisibleObject::SetRotation(Vector NewRotation)
{
	Rotation = NewRotation;
	if (RenderData != nullptr) RenderData->ApplyTransform();
}

void VisibleObject::SetScale(Vector NewScale)
{
	Scale = NewScale;
	if (RenderData != nullptr) RenderData->ApplyTransform();
}

void VisibleObject::SetModel(std::string Name)
{
	RenderData = MI->LoadData(this, Name);
	if (RenderData != nullptr) RenderData->ApplyTransform();
}
