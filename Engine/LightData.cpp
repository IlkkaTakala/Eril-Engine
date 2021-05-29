#include "LightData.h"

LightData::LightData()
{
	Type = 0;
	Intensity = 2.f;
	Size = 3.f;
	Distance = 0.f;
	Falloff = 1.f;
	Color = Vector(1.f, 1.f, 1.f);

	Location = Vector(0.f, 0.f, 0.f);
	Rotation = Vector(0.f, 0.f, 0.f);
}

Light::Light() : BaseObject()
{
	Data = LightData();
	Enabled = true;

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

void Light::Enable()
{
	if (!Enabled) {
		RI->CreateLight(&Data);
		Enabled = true;
	}
}

void Light::Disable()
{
	if (Enabled) {
		RI->RemoveLight(&Data);
		Enabled = false;
	}
}
