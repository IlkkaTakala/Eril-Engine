#pragma once
#include "Core.h"

class Texture;
class Shader;

class RenderBuffer
{
public:
	RenderBuffer(int width, int height);
	~RenderBuffer();

	void Bind();
	void Unbind();
	uint GetBuffer() const { return FrameBuffer; }
	uint GetDepth() const { return DepthBuffer; }
	uint GetColor() const { return ColorBuffer; }

	void BindTextures();
	void BindTexturesReading();

private:
	uint FrameBuffer;

	uint PositionBuffer;
	uint NormalBuffer;
	uint ColorBuffer;
	uint DepthBuffer;
	uint DataBuffer;
	uint SSAOBuffer;

	RenderBuffer(const RenderBuffer&);
	RenderBuffer& operator=(const RenderBuffer&);
};

class PostBuffer
{
public:
	PostBuffer(int width, int height);
	~PostBuffer();

	void Bind();
	void Unbind();
	uint GetBuffer() const { return FrameBuffer; }
	uint GetBloom() const { return BloomBuffer; }

	void BindTextures();

private:
	uint FrameBuffer;

	uint ColorBuffer;
	uint BloomBuffer;
	uint DepthBuffer;

	PostBuffer(const PostBuffer&);
	PostBuffer& operator=(const PostBuffer&);
};

class SSAOBuffer
{
public:
	SSAOBuffer(int width, int height);
	~SSAOBuffer();

	void Bind();
	void BindBlur();
	void Unbind();
	uint GetBuffer() const { return FrameBuffer; }
	uint GetSSAO() const { return ColorBuffer; }
	uint GetSSAOBlur() const { return BlurBuffer; }

	void BindTextures();

private:
	uint FrameBuffer;

	uint ColorBuffer;
	uint BlurBuffer;
	uint DepthBuffer;

	SSAOBuffer(const SSAOBuffer&);
	SSAOBuffer& operator=(const SSAOBuffer&);
};

class BlurBuffer
{
public:
	BlurBuffer(int width, int height, Shader* compute);
	~BlurBuffer();

	void Blur(uint texture, int samples);

private:
	Shader* BlurCompute;

	uint First;
	uint Second;

	int Width;
	int Height;

	BlurBuffer(const BlurBuffer&);
	BlurBuffer& operator=(const BlurBuffer&);
};

class ShadowBuffer
{
public:
	ShadowBuffer(int width, int height);
	~ShadowBuffer();

	void Bind();
	void Unbind();
	uint GetBuffer() const { return FrameBuffer; }
	uint GetShadows() const { return DepthBuffer; }

	void BindTextures();

private:
	uint FrameBuffer;

	uint DepthBuffer;

	ShadowBuffer(const ShadowBuffer&);
	ShadowBuffer& operator=(const ShadowBuffer&);
};