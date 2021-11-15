#include <UI/Panel.h>
#include "glad/gl.h"
#include "Material.h"
#include <glm/gtx/transform.hpp>

struct UIMatrix
{
	glm::mat4 model_m;
};

Panel::Panel() : UIComponent()
{
	const char* vertexShader = R"~~~(
#version 430 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_texCoord;
uniform mat4 model;
out vec2 TexCoords;
void main() {
	TexCoords = in_texCoord;
	gl_Position = model * vec4(in_position, 1.0);
}
)~~~";
	const char* fragmentShader = R"~~~(
#version 430 core
layout(location = 0) out vec4 FragColor;
in vec2 TexCoords;
void main() 
{ FragColor = vec4(0.5); }
)~~~";

	temp_shader = new Shader(vertexShader, fragmentShader);
}

Panel::~Panel()
{
	for (const auto& c : children) {
		delete c.second;
	}
}

Panel* Panel::AddChild(UIComponent* child)
{
	child->SetParent(this);
	auto it = children.emplace(child->GetZIndex(), child);
	float depth = realDepth;

	for (auto i = children.begin(); i != children.end(); i++) {
		i->second->UpdateDepth(depth);
	}

	return this;
}

void Panel::Render()
{
	for (auto it = children.rbegin(); it != children.rend(); it++) {
		it->second->Render();
	}
	temp_shader->Bind();
	if (recalculate) {
		recalculate = false;
	}
	//glm::mat4 view = glm::ortho(0.f, (float)size.X, (float)size.Y, 0.f, 100.f, 0.f);
	temp_shader->SetUniform("model", matrix->model_m);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Panel::UpdateDepth(float& depth)
{
	realDepth = depth;
	depth += 0.1f;

	for (auto i = children.begin(); i != children.end(); i++) {
		i->second->UpdateDepth(depth);
	}
	recalculate = true;
}

float Panel::GetTreeDepth() const
{
	if (children.size() > 0)
		return children.rbegin()->second->GetTreeDepth();
	return realDepth;
}
