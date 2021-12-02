#pragma once
#include <Core.h>
#include <Objects/VisibleObject.h>

class Terrain : public BaseObject
{
	REGISTER(Terrain);
public:
	Terrain();

	virtual void BeginPlay() override {}
	virtual void LoadWithParameters(const String& args) override;

	void InitTerrain(int r, Vector scale = Vector(1.f), Vector location = Vector(0.f), String material = "");
	float GetHeight(float x, float y);
	Vector GetNormal(float x, float y);
	Vector GetTangent(float x, float y, Vector normal);

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

