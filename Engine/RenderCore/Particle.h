#pragma once
#include "BasicTypes.h"

struct Particle
{
	Vector location;
	Vector scale;
	Vector rotation;
	Vector velocity;
	float lifetime;
	float max_lifetime;
	bool enabled;
	Vector colour;
	float alpha;
};