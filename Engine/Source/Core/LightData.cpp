#include "LightData.h"
/* Lights have been moved to be handled by the ECS-system. This is kept here if the code is needed later.

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

	IRender::CreateLight(&Data);
}

Light::~Light()
{
	IRender::RemoveLight(&Data);
}

void Light::OnDestroyed()
{
	IRender::RemoveLight(&Data);
}

void Light::Enable()
{
	if (!Enabled) {
		IRender::CreateLight(&Data);
		Enabled = true;
	}
}

void Light::Disable()
{
	if (Enabled) {
		IRender::RemoveLight(&Data);
		Enabled = false;
	}
}

*/