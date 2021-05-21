#pragma once
#include "Core.h"
#include "IRender.h"

struct LightData 
{
	LightData();

	int Type;

	float Intensity;
	float Size;
	float Distance;
	float Falloff;
	Vector Color;

	Vector Location;
	Vector Rotation;
};

class Light : public BaseObject
{
public:
	Light();
	~Light();

	virtual void DestroyObject() override;
	virtual void BeginPlay() override {};

	LightData Data;
	
private:
	
};

