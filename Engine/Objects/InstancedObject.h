#pragma once
#include "Objects/BaseObject.h"

class InstancedObject : public BaseObject
{
public:
	InstancedObject();

	virtual void BeginPlay() override;
};

