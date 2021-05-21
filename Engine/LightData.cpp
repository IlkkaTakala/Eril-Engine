#include "LightData.h"

LightData::LightData()
{
	Type = 0;
	Intensity = 10.f;
	Size = 1.f;
	Distance = 0.f;
	Falloff = 1.f;
	Color = Vector(1.f, 1.f, 1.f);

	Location = Vector(0.f, 0.f, 10.f);
	Rotation = Vector(0.f, 0.f, 0.f);
}

Light::Light() : BaseObject()
{
	Data = LightData();

	RI->CreateLight(&Data);
}

Light::~Light()
{
	RI->RemoveLight(&Data);
}

void Light::DestroyObject()
{
	RI->RemoveLight(&Data);
}