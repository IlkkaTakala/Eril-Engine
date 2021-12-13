#pragma once
#include "Core.h"

class Texture
{
public:
	Texture(int width, int height, int nrChannels, const uint8* data, int type);
	Texture(int width, int height, int nrChannels, const float* data);
	Texture(int width, int height, bool isDepthTexture);
	~Texture();

	uint GetTextureID() const { return ID; }
	String GetName() const { return Name; }
	void SetName(const String& name) { if (Name == "") Name = name; }
private:
	uint ID;
	int Type;
	String Name;
};