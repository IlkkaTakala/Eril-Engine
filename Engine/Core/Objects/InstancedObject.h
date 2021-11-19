#pragma once
#include "Core.h"
#include "Objects/VisibleObject.h"

class InstancedObject : public VisibleObject
{
	REGISTER(InstancedObject)
public:
	InstancedObject();

	void AddInstances(int count, Transformation* dispArray);
};

