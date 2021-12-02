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

private:
	uint ID;
	int Type;
};