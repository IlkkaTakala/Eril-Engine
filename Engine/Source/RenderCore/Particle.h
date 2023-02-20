#pragma once
#include "BasicTypes.h"

struct Particle
{
	bool enabled;

	Vector location;
	Vector scale;
	Rotator rotation;
	Vector rotationRate;
	Vector velocity;
	Vector colour;
	float alpha;
	float lifetime;

	float max_lifetime;
	Vector initialScale;
	Vector initialColour;
};