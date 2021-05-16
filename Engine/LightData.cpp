#include "LightData.h"

LightData::LightData() : BaseObject()
{
	Type = 0;
	Intensity = 10.f;
	Size = 10.f;
	Distance = 0.f;
	Falloff = 1.f;
	Color = Vector(1.f, 0.f, 0.f);

	Location = Vector(0.f, 0.f, 0.f);
	Rotation = Vector(0.f, 0.f, 0.f);

	RI->CreateLight(this);
}

LightData::~LightData()
{
	RI->RemoveLight(this);
}

void LightData::DestroyObject()
{
	RI->RemoveLight(this);
}
