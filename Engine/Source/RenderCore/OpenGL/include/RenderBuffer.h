#pragma once
#include "Core.h"

class Texture;
class Shader;

class PreDepthBuffer
{
public:
	PreDepthBuffer(int width, int height);
	~PreDepthBuffer();

	void Bind();
	void Unbind();
	uint GetBuffer() const { return FrameBuffer; }
	uint GetDepth() const { return DepthBuffer; }

	void BindTextures(int offset = 0);
	void BindTexturesReading();

private:
	uint FrameBuffer;
	uint NormalBuffer;
	uint DepthBuffer;

	PreDepthBuffer(const PreDepthBuffer&);
	PreDepthBuffer& operator=(const PreDepthBuffer&);
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
	uint GetNormal() const { return NormalBuffer; }
	uint GetColor() const { return ColorBuffer; }
	uint GetPosition() const { return PositionBuffer; }
	uint GetData() const { return DataBuffer; }

	void BindTextures(int offset = 0);

private:
	uint FrameBuffer;

	uint ColorBuffer;
	uint BloomBuffer;
	uint AccumBuffer;
	uint RevealageBuffer;
	uint DepthBuffer;
	uint NormalBuffer;
	uint PositionBuffer;
	uint DataBuffer;

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

	void BindTextures(int offset = 0);

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

	void Blur(uint texture, uint samples, uint quad);

private:
	Shader* BlurCompute;

	uint pingpongFBO;
	uint pingpongBuffer[2];

	int Width;
	int Height;

	BlurBuffer(const BlurBuffer&);
	BlurBuffer& operator=(const BlurBuffer&);
};

class ShadowMapBuffer
{
public:
	ShadowMapBuffer(int width, int height);
	~ShadowMapBuffer();

	void Bind();
	void Unbind();
	uint GetBuffer() const { return FrameBuffer; }
	uint GetDepth() const { return DepthBuffer; }
	uint GetShadows() const { return Shadow; }

private:
	uint FrameBuffer;

	uint Shadow;
	uint DepthBuffer;

	ShadowMapBuffer(const ShadowMapBuffer&);
	ShadowMapBuffer& operator=(const ShadowMapBuffer&);
};

class ReflectionBuffer
{
public:
	ReflectionBuffer(int width, int height);
	~ReflectionBuffer();

	void Bind();
	void BindFilter();
	void Unbind();
	uint GetBuffer() const { return FrameBuffer; }
	uint GetEnvironment() const { return Env; }
	uint GetFilterEnvironment() const { return Filter; }

	void BindTextures(int offset = 0);

private:
	uint FrameBuffer;
	uint FrameBufferFilter;

	uint Env;
	uint Filter;

	ReflectionBuffer(const ReflectionBuffer&);
	ReflectionBuffer& operator=(const ReflectionBuffer&);
};