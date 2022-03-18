#pragma once
#include "Core.h"

class Texture
{
public:
	Texture();
	Texture(int width, int height, int nrChannels, const uint8* data, int type);
	Texture(int width, int height, int nrChannels, const float* data);
	~Texture();

	uint GetTextureID() const { return ID; }
	String GetName() const { return Name; }
	void SetName(const String& name) { if (Name == "") Name = name; }
	void MakeBuffers();
private:
	uint ID;
	int Type;
	int DataType;
	int Width;
	int Height;
	int Initialized;
	int Channels;
	String Name;
	void* Data;
};