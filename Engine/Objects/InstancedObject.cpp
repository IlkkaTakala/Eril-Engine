#include "InstancedObject.h"

InstancedObject::InstancedObject() : VisibleObject()
{

}

void InstancedObject::AddInstances(int count, Transformation* dispArray)
{
	if (RenderData != nullptr) RenderData->SetInstances(count, dispArray);
}
