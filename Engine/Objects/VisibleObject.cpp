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
	RenderData->ApplyTransform();
}

void VisibleObject::SetRotation(Vector NewRotation)
{
	Rotation = NewRotation;
	RenderData->ApplyTransform();
}

void VisibleObject::SetScale(Vector NewScale)
{
	Scale = NewScale;
	RenderData->ApplyTransform();
}

void VisibleObject::SetModel(std::string Name)
{
	RenderData = MI->LoadData(this, Name);
	RenderData->ApplyTransform();
}
