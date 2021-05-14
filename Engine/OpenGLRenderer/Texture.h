#pragma once
#include "Core.h"

class Texture
{
public:
	Texture(int width, int height, int nrChannels, const uint8* data);
	Texture(int width, int height, bool isDepthTexture);
	~Texture();

	uint GetTextureID() const { return ID; }

private:
	uint ID;
};