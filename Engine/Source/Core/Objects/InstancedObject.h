#pragma once
#include "Core.h"
#include "Objects/VisibleObject.h"

class InstancedObject : public VisibleObject
{
	REGISTER(InstancedObject)
public:
	InstancedObject();
	virtual void LoadWithParameters(const String& args) override;

	void AddInstances(int count, Transformation* dispArray);
};

