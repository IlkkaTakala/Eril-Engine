#include "UISpace.h"
#include <glad/gl.h>
#include <UI/UI.h>
#include "UIComponent.h"
#include <Material.h>
#include <glm/gtx/transform.hpp>

UISpace::UISpace()
{
	UIBuffer = 0;
	Color = 0;
	Depth = 0;
	UIShader = nullptr;

	const char* vertexShader = R"~~~(
#version 430 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_texCoord;
out vec2 TexCoords;
void main() {
	TexCoords = in_texCoord;
	gl_Position = vec4(in_position, 1.0);
}
)~~~";
	const char* fragmentShader = R"~~~(
#version 430 core
out vec4 FragColor;
layout (binding = 0) uniform sampler2D Color;
in vec2 TexCoords;
void main() 
{ FragColor = texture(Color, TexCoords); }
)~~~";


	Combiner = new Shader(vertexShader, fragmentShader);

	float texCoords[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};

	float vertices[] = {
		-1.f, -1.f, 1.f,
		1.f, -1.f, 1.f,
		1.f, 1.f, 1.f,
		-1.f, -1.f, 1.f,
		1.f, 1.f, 1.f,
		-1.f, 1.f, 1.f
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &TBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, TBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

UISpace::~UISpace()
{
	delete Combiner;
	delete UIShader;

	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &TBO);
	glDeleteVertexArrays(1, &VAO);

	if (UIBuffer != 0) {
		glDeleteTextures(1, &Color);
		glDeleteFramebuffers(1, &UIBuffer);
	}

	for (const auto& t : TopLevel) {
		delete t;
	}
}

void UISpace::SetSize(uint width, uint height)
{
	ScreenSize = Vector2D(width, height);

	glGenFramebuffers(1, &UIBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, UIBuffer);

	// - normal color buffer
	glGenTextures(1, &Color);
	glBindTexture(GL_TEXTURE_2D, Color);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Color, 0);

	// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachments);

	glGenTextures(1, &Depth);
	glBindTexture(GL_TEXTURE_2D, Depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, Depth, 0);
	GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);


	if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
	{
		throw std::runtime_error("Could not initialize UI Renderer!");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void UISpace::Render(uint target)
{
	glBindVertexArray(VAO);
	
	glBindFramebuffer(GL_FRAMEBUFFER, UIBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	glDisable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);
	for (const auto& c : TopLevel) {
		c->TopLevel->Render();
	}

	Combiner->Bind();
	glBindFramebuffer(GL_FRAMEBUFFER, target);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Color);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void UISpace::AddComponent(UI* com)
{
	com->PreConstruct();
	com->Construct();
	if (com->TopLevel != nullptr)
		com->TopLevel->UpdateMatrices(ScreenSize);
	com->PostConstruct();
	TopLevel.push_back(com);
}
