#pragma once
#include <Core.h>
#include <Objects/VisibleObject.h>

class Terrain : public BaseObject
{
public:
	Terrain();

	virtual void BeginPlay() override {}

	void InitTerrain(int r, Vector scale = Vector(1.f), Vector location = Vector(0.f));
	float GetHeight(float x, float y);
private:
	virtual ~Terrain() {};

	int resolution;
	Vector Scale;

	float noise_scale;
	float amplitude;

	bool isTexture;
	Texture* Heightmap;

	Ref<VisibleObject> Mesh;
};

