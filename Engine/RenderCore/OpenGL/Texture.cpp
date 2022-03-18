#include "Texture.h"
#include <glad/gl.h>

Texture::Texture()
{
	ID = 0;
	Type = 0;
	Name = "";
	Data = nullptr;
	DataType = 0;
	Width = 0;
	Height = 0;
	Initialized = false;
	Channels = 0;
}


Texture::Texture(int width, int height, int nrChannels, const uint8* data, int type)
{
	ID = 0;
	Type = type;
	Name = "";
	DataType = 1;
	Width = width;
	Height = height;
	Initialized = false;
	Channels = nrChannels;
	Data = new uint8[width * height * Channels]();
	memcpy(Data, data, width * height * Channels);
}

Texture::Texture(int width, int height, int nrChannels, const float* data)
{
	ID = 0;
	Type = 0;
	Name = "";
	DataType = 2;
	Width = width;
	Height = height;
	Initialized = false;
	Channels = nrChannels;
	Data = new float[width * height * Channels]();
	memcpy(Data, data, width * height * Channels);
}

Texture::~Texture()
{
	delete[] Data;
	glDeleteTextures(1, &ID);
}

void Texture::MakeBuffers()
{
	switch (DataType)
	{
	case 1: {
		glGenTextures(1, &ID);
		glBindTexture(GL_TEXTURE_2D, ID);
		switch (Channels)
		{
		case 1:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, Width, Height, 0, GL_RED, GL_UNSIGNED_BYTE, Data);
			break;
		case 3:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Data);
			break;
		case 4:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Data);
			break;
		default:
			break;
		}
		switch (Type)
		{
		case 0:
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			GLfloat value, max_anisotropy = 16.0f;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &value);

			value = (value > max_anisotropy) ? max_anisotropy : value;
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, value);
		}
		break;
		case 1:
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			GLfloat value, max_anisotropy = 16.0f;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &value);

			value = (value > max_anisotropy) ? max_anisotropy : value;
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, value);
		}
		break;
		case 2:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			break;
		default:
			break;
		}
	} break;

	case 2: {
		glGenTextures(1, &ID);
		glBindTexture(GL_TEXTURE_2D, ID);
		switch (Channels)
		{
		case 1:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, Width, Height, 0, GL_RED, GL_FLOAT, Data);
			break;
		case 3:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Width, Height, 0, GL_RGB, GL_FLOAT, Data);
			break;
		case 4:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Width, Height, 0, GL_RGBA, GL_FLOAT, Data);
			break;
		default:
			break;
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		GLfloat value, max_anisotropy = 16.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &value);

		value = (value > max_anisotropy) ? max_anisotropy : value;
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, value);
	} break;

	default:
		break;
	}
}
