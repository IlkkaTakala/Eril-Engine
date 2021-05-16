#pragma once
#include "Core.h"

class Texture;

class RenderBuffer
{
public:
	RenderBuffer(int width, int height);
	~RenderBuffer();

	void Bind();
	void Unbind();
	uint GetBuffer() const { return FrameBuffer; }

	void BindTextures();

private:
	uint FrameBuffer;

	uint PositionBuffer;
	uint NormalBuffer;
	uint ColorBuffer;
	uint DepthBuffer;
	uint DataBuffer;

	RenderBuffer(const RenderBuffer&);
	RenderBuffer& operator=(const RenderBuffer&);
};