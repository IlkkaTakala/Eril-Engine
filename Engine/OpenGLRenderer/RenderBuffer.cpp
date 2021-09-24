#include "RenderBuffer.h"
#include <glad/gl.h>
#include "LightData.h"
#include "Material.h"
#include <stdexcept>

PreDepthBuffer::PreDepthBuffer(int width, int height)
{

	glGenFramebuffers(1, &FrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);

	// - normal color buffer
	glGenTextures(1, &NormalBuffer);
	glBindTexture(GL_TEXTURE_2D, NormalBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, NormalBuffer, 0);

	// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachments);

	glGenTextures(1, &DepthBuffer);
	glBindTexture(GL_TEXTURE_2D, DepthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthBuffer, 0);
	GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
	{
		throw std::runtime_error("Texture could not add texture to framebuffer!");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

PreDepthBuffer::~PreDepthBuffer()
{
	glDeleteTextures(1, &NormalBuffer);
	glDeleteTextures(1, &DepthBuffer);
	glDeleteFramebuffers(1, &FrameBuffer);
}

void PreDepthBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
}

void PreDepthBuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PreDepthBuffer::BindTextures()
{
	//unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	//glDrawBuffers(4, attachments);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, DepthBuffer);
}

void PreDepthBuffer::BindTexturesReading()
{
	uint textures[1] = { NormalBuffer };
	glBindImageTextures(0, 1, textures);
}

BlurBuffer::BlurBuffer(int width, int height, Shader* compute)
{

	BlurCompute = compute;
	Width = width;
	Height = height;

	glGenFramebuffers(1, &pingpongFBO);
	glGenTextures(2, pingpongBuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO);
	glBindTexture(GL_TEXTURE_2D, pingpongBuffer[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Width / 2, Height / 2, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[0], 0
	);
	glBindTexture(GL_TEXTURE_2D, pingpongBuffer[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Width / 2, Height / 2, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, pingpongBuffer[1], 0
	);
}

BlurBuffer::~BlurBuffer()
{
	glDeleteTextures(1, &pingpongBuffer[0]);
	glDeleteTextures(1, &pingpongBuffer[1]);
	glDeleteFramebuffers(1, &pingpongFBO);
}

void BlurBuffer::Blur(uint texture, uint samples, uint quad)
{
	glViewport(0, 0, Width / 2, Height / 2);
	glDisable(GL_DEPTH_TEST);
	bool horizontal = true, first_iteration = true;
	glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO);
	BlurCompute->Bind();
	BlurCompute->SetUniform("image", 0);
	glBindVertexArray(quad);
	glActiveTexture(GL_TEXTURE0);
	for (unsigned int i = 0; i < samples * 2 - 1; i++)
	{
		glDrawBuffer(GL_COLOR_ATTACHMENT0 + horizontal);
		BlurCompute->SetUniform("horizontal", horizontal);
		glBindTexture(
			GL_TEXTURE_2D, first_iteration ? texture : pingpongBuffer[!horizontal]
		);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}

	glViewport(0, 0, Width, Height);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, NULL, 0);

	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glBindTexture(GL_TEXTURE_2D, pingpongBuffer[!horizontal]);
	BlurCompute->SetUniform("horizontal", horizontal);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[0], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, pingpongBuffer[1], 0);

	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
}

PostBuffer::PostBuffer(int width, int height)
{
	glGenFramebuffers(1, &FrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);

	// - color buffer
	glGenTextures(1, &ColorBuffer);
	glBindTexture(GL_TEXTURE_2D, ColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorBuffer, 0);

	// - bloom color buffer
	glGenTextures(1, &BloomBuffer);
	glBindTexture(GL_TEXTURE_2D, BloomBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, BloomBuffer, 0);

	// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	glGenTextures(1, &DepthBuffer);
	glBindTexture(GL_TEXTURE_2D, DepthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthBuffer, 0);
	GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
	{
		throw std::runtime_error("Texture could not add texture to framebuffer! : Post Process\n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

PostBuffer::~PostBuffer()
{
	glDeleteTextures(1, &ColorBuffer);
	glDeleteTextures(1, &BloomBuffer);
	glDeleteTextures(1, &DepthBuffer);
	glDeleteFramebuffers(1, &FrameBuffer);
}

void PostBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
	//unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	//glDrawBuffers(2, attachments);
}

void PostBuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostBuffer::BindTextures()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ColorBuffer);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, BloomBuffer);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, DepthBuffer);
}

TransparencyBuffer::TransparencyBuffer(int width, int height)
{
	glGenFramebuffers(1, &FrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);

	// - accumulation color buffer
	glGenTextures(1, &AccumBuffer);
	glBindTexture(GL_TEXTURE_2D, AccumBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, AccumBuffer, 0);

	// - revealage color buffer
	glGenTextures(1, &RevealageBuffer);
	glBindTexture(GL_TEXTURE_2D, RevealageBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, RevealageBuffer, 0);

	// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
	{
		throw std::runtime_error("Texture could not add texture to framebuffer! : Post Process\n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

TransparencyBuffer::~TransparencyBuffer()
{
	glDeleteTextures(1, &AccumBuffer);
	glDeleteTextures(1, &RevealageBuffer);
	glDeleteFramebuffers(1, &FrameBuffer);
}

void TransparencyBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
	//unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	//glDrawBuffers(2, attachments);
}

void TransparencyBuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TransparencyBuffer::BindTextures()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, AccumBuffer);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, RevealageBuffer);
}

SSAOBuffer::SSAOBuffer(int width, int height)
{
	glGenFramebuffers(1, &FrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);

	glGenTextures(1, &ColorBuffer);
	glBindTexture(GL_TEXTURE_2D, ColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, ColorBuffer, 0);

	glGenTextures(1, &BlurBuffer);
	glBindTexture(GL_TEXTURE_2D, BlurBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, BlurBuffer, 0);

	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	glGenTextures(1, &DepthBuffer);
	glBindTexture(GL_TEXTURE_2D, DepthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthBuffer, 0);
	GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
	{
		throw std::runtime_error("Texture could not add texture to framebuffer!");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

SSAOBuffer::~SSAOBuffer()
{
	glDeleteTextures(1, &ColorBuffer);
	glDeleteTextures(1, &DepthBuffer);
	glDeleteTextures(1, &BlurBuffer);
	glDeleteFramebuffers(1, &FrameBuffer);
}

void SSAOBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
}

void SSAOBuffer::BindBlur()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
	unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachments);
}

void SSAOBuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAOBuffer::BindTextures()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ColorBuffer);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, BlurBuffer);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, DepthBuffer);
}

ShadowBuffer::ShadowBuffer(int width, int height)
{
	glGenFramebuffers(1, &FrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);

	glGenTextures(1, &Shadow);
	glBindTexture(GL_TEXTURE_2D, Shadow);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Shadow, 0);

	glGenTextures(1, &DepthBuffer);
	glBindTexture(GL_TEXTURE_2D, DepthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, DepthBuffer, 0);

	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
	{
		throw std::runtime_error("Texture could not add texture to framebuffer!\n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ShadowBuffer::~ShadowBuffer()
{
	glDeleteTextures(1, &DepthBuffer);
	glDeleteFramebuffers(1, &FrameBuffer);
}

void ShadowBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
}

void ShadowBuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowBuffer::BindTextures()
{
}

ReflectionBuffer::ReflectionBuffer(int width, int height)
{
	glGenFramebuffers(1, &FrameBuffer);
	glGenFramebuffers(1, &FrameBufferFilter);
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);

	glGenTextures(1, &Env);
	glBindTexture(GL_TEXTURE_CUBE_MAP, Env);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);
	for (uint face = 0; face < 6; face++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGBA16F,
			width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	}
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, Env, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glBindFramebuffer(GL_FRAMEBUFFER, FrameBufferFilter);
	glGenTextures(1, &Filter);
	glBindTexture(GL_TEXTURE_CUBE_MAP, Filter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 5);
	for (uint face = 0; face < 6; face++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGBA8,
			width / 2, height / 2, 0, GL_RGBA, GL_FLOAT, NULL);
	}
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, Filter, 0);

	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
	{
		throw std::runtime_error("Texture could not add texture to framebuffer! Reflections \n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ReflectionBuffer::~ReflectionBuffer()
{
	glDeleteTextures(1, &Env);
	glDeleteTextures(1, &Filter);
	glDeleteFramebuffers(1, &FrameBuffer);
}

void ReflectionBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
}

void ReflectionBuffer::BindFilter()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBufferFilter);
}

void ReflectionBuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ReflectionBuffer::BindTextures()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, Env);
}

ShadowMapBuffer::ShadowMapBuffer(int width, int height)
{
	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

	glGenFramebuffers(1, &FrameBuffer);
	glGenTextures(1, &DepthBuffer);
	glBindTexture(GL_TEXTURE_2D, DepthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthBuffer, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ShadowMapBuffer::~ShadowMapBuffer()
{
	glDeleteTextures(1, &DepthBuffer);
	glDeleteFramebuffers(1, &FrameBuffer);
}

void ShadowMapBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
}

void ShadowMapBuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
