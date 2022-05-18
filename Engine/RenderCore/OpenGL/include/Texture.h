#pragma once
#include "Core.h"
#include "../OpenGLObject.h"

class Texture : public OpenGLObject
{
public:
	Texture();
	Texture(int width, int height, int nrChannels, const uint8* data, int type);
	Texture(int width, int height, int nrChannels, const float* data);
	~Texture();

	uint GetTextureID() const { return ID; }
	String GetName() const { return Name; }
	void SetName(const String& name) { if (Name == "") Name = name; }

	virtual void CreateState() override;
	virtual void Clear() override;

	void LoadTexture(int width, int height, int nrChannels, const uint8* data, int type);
	void LoadTexture(int width, int height, int nrChannels, const float* data);
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