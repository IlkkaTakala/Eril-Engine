#include <UI/Image.h>
#include "Material.h"
#include "Texture.h"

struct UIMatrix
{
	glm::mat4 model_m;
};

struct UIStyleGLM
{
	glm::vec4 color;
	glm::vec4 tint;

	UIStyleGLM(const UIStyle& old)
	{
		color = glm::vec4(old.Color.X, old.Color.Y, old.Color.Z, old.Opacity);
		tint = glm::vec4(old.Tint.X, old.Tint.Y, old.Tint.Z, 1.f);
	}
};

Image::Image()
{
	const char* vertexShader = R"~~~(
#version 430 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_texCoord;
uniform mat4 model;
out vec2 TexCoords;
void main() {
	TexCoords = in_texCoord;
	vec4 loc = model * vec4((in_position + 1.0) / 2.0, 1.0);
	loc.z = model[3][2] - 1.0;
	gl_Position = vec4(loc.xyz, 1.0);
}
)~~~";
	const char* solidShader = R"~~~(
#version 430 core
out vec4 FragColor;
in vec2 TexCoords;
layout (std140, binding = 1) uniform style
{
	vec4 color;
	vec4 tint;
};
void main() 
{ FragColor = color * tint; }
)~~~";
	const char* textureShader = R"~~~(
#version 430 core
out vec4 FragColor;
in vec2 TexCoords;
layout (std140, binding = 1) uniform style
{
	vec4 color;
	vec4 tint;
};
uniform sampler2D tex;
void main() 
{ 
	vec4 res = texture(tex, TexCoords) * tint;
	res.a *= color.a;
	FragColor = res; 
}
)~~~";

	solid_shader = new Shader(vertexShader, solidShader);
	texture_shader = new Shader(vertexShader, textureShader);

	UIStyleGLM n(style);
	glGenBuffers(1, &uniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(UIStyleGLM), &n, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	hits = HitReg::HitTestVisible;
}

Image::~Image()
{
	delete solid_shader;
	delete texture_shader;

	delete style.texture;
}

void Image::Render()
{
	if (visible != Visibility::Visible) return;
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, uniformBuffer);
	if (style.texture == nullptr) {
		solid_shader->Bind();
		solid_shader->SetUniform("model", matrix->model_m);
	}
	else {
		texture_shader->Bind();
		texture_shader->SetUniform("model", matrix->model_m);
		texture_shader->SetUniform("tex", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, style.texture->GetTextureID());
	}
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

Image* Image::SetStyle(const UIStyle& s)
{
	style = s;

	UIStyleGLM n(style);
	glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(UIStyleGLM), &n);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	return this;
}
