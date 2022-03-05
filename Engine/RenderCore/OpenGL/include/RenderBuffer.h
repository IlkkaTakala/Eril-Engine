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

	void BindTextures();
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
	uint GetColor() const { return ColorBuffer; }

	void BindTextures();

private:
	uint FrameBuffer;

	uint ColorBuffer;
	uint BloomBuffer;
	uint AccumBuffer;
	uint RevealageBuffer;
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

class ShadowBuffer
{
public:
	ShadowBuffer(int width, int height);
	~ShadowBuffer();

	void Bind();
	void Unbind();
	uint GetBuffer() const { return FrameBuffer; }
	uint GetShadows() const { return Shadow; }

	void BindTextures();

private:
	uint FrameBuffer;

	uint Shadow;
	uint DepthBuffer;

	ShadowBuffer(const ShadowBuffer&);
	ShadowBuffer& operator=(const ShadowBuffer&);
};

class ShadowMapBuffer
{
public:
	ShadowMapBuffer(int width, int height);
	~ShadowMapBuffer();

	void Bind();
	void Unbind();
	uint GetBuffer() const { return FrameBuffer; }
	uint GetShadows() const { return DepthBuffer; }

private:
	uint FrameBuffer;

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

	void BindTextures();

private:
	uint FrameBuffer;
	uint FrameBufferFilter;

	uint Env;
	uint Filter;

	ReflectionBuffer(const ReflectionBuffer&);
	ReflectionBuffer& operator=(const ReflectionBuffer&);
};