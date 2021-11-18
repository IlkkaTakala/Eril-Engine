#pragma once
#include "Core.h"
#include "Interface/IRender.h"

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

constexpr int LIGHT_DIRECTIONAL = 0;
constexpr int LIGHT_POINT = 1;
constexpr int LIGHT_SPOT = 2;

class Light : public BaseObject
{
public:
	Light();
	~Light();

	virtual void OnDestroyed() override;
	virtual void BeginPlay() override {};

	void Enable();
	void Disable();

	LightData Data;
	bool Enabled;
	
private:
	
};

