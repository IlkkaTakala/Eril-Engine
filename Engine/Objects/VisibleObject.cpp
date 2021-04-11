#include "VisibleObject.h"

VisibleObject::VisibleObject() : BaseObject()
{
	Location = Vector(0, 0, 0);
	Rotation = Vector(0, 0, 0);
	Scale = Vector(1, 1, 1);
}

void VisibleObject::SetLocation(Vector NewLocation)
{
	Location = NewLocation;
}

void VisibleObject::SetRotation(Vector NewRotation)
{
	Rotation = NewRotation;
}

void VisibleObject::SetScale(Vector NewScale)
{
	Scale = NewScale;
}

void VisibleObject::SetModel(std::string Name)
{
	
}
