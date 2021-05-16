#pragma once
#include "Core.h"

class LightData : public BaseObject
{
public:
	LightData();
	~LightData();

	virtual void DestroyObject() override;

	uint8 Type;

	float Intensity;
	float Size;
	float Distance;
	float Falloff;
	Vector Color;

	Vector Location;
	Vector Rotation;
private:
	
};

