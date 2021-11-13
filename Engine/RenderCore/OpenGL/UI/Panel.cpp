#include "Panel.h"
#include "glad/gl.h"
#include "Material.h"

Panel::Panel() : UIComponent()
{
	const char* vertexShader = R"~~~(
#version 430 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_texCoord;
out vec2 TexCoords;
void main() {
	TexCoords = in_texCoord;
	gl_Position = vec4(in_position / 2.f, 1.0);
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

void Panel::AddChild(UIComponent* child)
{
	child->SetParent(this);
	auto it = children.emplace(child->GetZIndex(), child);
	float depth = realDepth;

	for (auto i = children.begin(); i != children.end(); i++) {
		i->second->UpdateDepth(depth);
	}
}

void Panel::Render(const Vector2D& size) const
{
	for (auto it = children.rbegin(); it != children.rend(); it++) {
		it->second->Render(size);
	}
	temp_shader->Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Panel::UpdateDepth(float& depth)
{
	realDepth = depth;
	depth += 0.1f;

	for (auto i = children.begin(); i != children.end(); i++) {
		i->second->UpdateDepth(depth);
	}
}

float Panel::GetTreeDepth() const
{
	if (children.size() > 0)
		return children.rbegin()->second->GetTreeDepth();
	return realDepth;
}
