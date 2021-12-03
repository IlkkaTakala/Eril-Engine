#include <UI/Button.h>
#include "Material.h"
#include "Texture.h"
#include <UI/Text.h>

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

Button::Button()
{
	normalBuffer = 0;
	hoverBuffer = 0;
	pressBuffer = 0;
	state = ButtonState::Neutral;
	child = nullptr;
	focusable = true;

	glDeleteBuffers(1, &uniformBuffer);

	normal = UIStyle(Vector(1.f));
	UIStyleGLM n(style);
	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, normalBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(UIStyleGLM), &n, GL_DYNAMIC_DRAW);

	hovered = UIStyle(Vector(0.8f));
	UIStyleGLM h(hovered);
	glGenBuffers(1, &hoverBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, hoverBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(UIStyleGLM), &h, GL_DYNAMIC_DRAW);

	pressed = UIStyle(Vector(0.5f));
	UIStyleGLM p(pressed);
	glGenBuffers(1, &pressBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, pressBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(UIStyleGLM), &p, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	Console::Log("Button here");
}

Button::~Button()
{
	delete normal.texture;
	delete hovered.texture;
	delete pressed.texture;

	glDeleteBuffers(1, &normalBuffer);
	glDeleteBuffers(1, &hoverBuffer);
	glDeleteBuffers(1, &pressBuffer);

	delete child;
}

Button* Button::SetHoverStyle(const UIStyle& s)
{
	hovered = s;

	UIStyleGLM n(hovered);
	glBindBuffer(GL_UNIFORM_BUFFER, hoverBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(UIStyleGLM), &n);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	return this;
}

Button* Button::SetPressStyle(const UIStyle& s)
{
	pressed = s;

	UIStyleGLM n(pressed);
	glBindBuffer(GL_UNIFORM_BUFFER, pressBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(UIStyleGLM), &n);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	return this;
}

Button* Button::SetStyle(const UIStyle& s)
{
	normal = s;

	UIStyleGLM n(normal);
	glBindBuffer(GL_UNIFORM_BUFFER, normalBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(UIStyleGLM), &n);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	return this;
}

void Button::OnEnter()
{
	state = ButtonState::Hovered;
	Image::OnEnter();
}

void Button::OnLeave()
{
	state = ButtonState::Neutral;
	Image::OnLeave();
}

void Button::OnMouseDown()
{
	state = ButtonState::Pressed;
	Image::OnMouseDown();
}

void Button::OnMouseUp()
{
	state = ButtonState::Hovered;
	Image::OnMouseUp();
}

void Button::Render()
{
	switch (state)
	{
	case ButtonState::Neutral: uniformBuffer = normalBuffer;
		break;
	case ButtonState::Hovered: uniformBuffer = hoverBuffer;
		break;
	case ButtonState::Pressed: uniformBuffer = pressBuffer;
		break;
	default:
		break;
	}

	Image::Render();

	if (child != nullptr) child->Render();
}

void Button::UpdateDepth(float& depth)
{
	realDepth = depth;
	depth += 0.1f;

	if (child != nullptr) child->UpdateDepth(depth);
}

float Button::GetTreeDepth() const
{
	if (child != nullptr) return child->GetTreeDepth();
	return realDepth;
}

void Button::UpdateMatrices(const Vector2D& size)
{
	Image::UpdateMatrices(size);

	if (child != nullptr) {
		child->SetTransform(0.f, 0.f, 0.f, 0.f, Vector(0.f, 1.f, 0.f), Vector(0.f, 1.f, 0.f));
		child->UpdateMatrices(realSize);
	}
}

void Button::HoverCheck(Vector2D& point)
{
	if (child != nullptr) child->HoverCheck(point);
	Image::HoverCheck(point);
}

Button* Button::AddChild(UIComponent* c)
{
	child = c;
	Text* t = dynamic_cast<Text*>(child);
	if (t != nullptr) t->SetJustification(Justify::Centre);
	child->SetParent(this);
	float depth = realDepth + 0.1f;
	child->UpdateDepth(depth);
	return this;
}
