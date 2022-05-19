#pragma once
#include "Core.h"

class LightSpawner : public BaseObject
{
	REGISTER(LightSpawner);
public:
	LightSpawner();
	void BeginPlay() override {}
	virtual void LoadWithParameters(const String& args) override;


};