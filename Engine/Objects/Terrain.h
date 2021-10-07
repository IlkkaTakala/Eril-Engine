#pragma once
#include <Core.h>
#include <Objects/VisibleObject.h>

class Terrain : public BaseObject
{
public:
	Terrain();

	virtual void BeginPlay() override {}

	void InitTerrain(int r, Vector scale = Vector(1.f));
	float GetHeight(float x, float y);
private:

	int resolution;
	Vector Scale;

	float noise_scale = 0.010;
	float amplitude = 10.0;

	bool isTexture;
	Texture* Heightmap;

	Ref<VisibleObject> Mesh;
};

