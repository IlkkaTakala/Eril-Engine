#include <UI/Button.h>
#include "Material.h"
#include "Texture.h"
#include <UI/Text.h>

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

Button::Button()
{
	normalBuffer = 0;
	hoverBuffer = 0;
	pressBuffer = 0;
	state = ButtonState::Neutral;
	child = nullptr;
	focusable = true;

	glDeleteBuffers(1, &uniformBuffer);

	normal = UIStyle(Vector(1.f), 1.f, RI->LoadTextureByName("Assets/Textures/button.png"));
	UIStyleGLM n(normal);
	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, normalBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(UIStyleGLM), &n, GL_DYNAMIC_DRAW);

	hovered = UIStyle(Vector(0.8f), 1.f, RI->LoadTextureByName("Assets/Textures/button.png"));
	UIStyleGLM h(hovered);
	glGenBuffers(1, &hoverBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, hoverBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(UIStyleGLM), &h, GL_DYNAMIC_DRAW);

	pressed = UIStyle(Vector(0.8f), 1.f, RI->LoadTextureByName("Assets/Textures/buttonPressed.png"));
	UIStyleGLM p(pressed);
	glGenBuffers(1, &pressBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, pressBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(UIStyleGLM), &p, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

Button::~Button()
{
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
	UIStyle* current = nullptr;
	switch (state)
	{
	case ButtonState::Neutral: uniformBuffer = normalBuffer; current = &normal;
		break;
	case ButtonState::Hovered: uniformBuffer = hoverBuffer; current = &hovered;
		break;
	case ButtonState::Pressed: uniformBuffer = pressBuffer; current = &pressed;
		break;
	default:
		break;
	}

	if (visible != Visibility::Visible) return;
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, uniformBuffer);
	if (current->texture == nullptr) {
		solid_shader->Bind();
		solid_shader->SetUniform("model", matrix->model_m);
	}
	else {
		texture_shader->Bind();
		texture_shader->SetUniform("model", matrix->model_m);
		texture_shader->SetUniform("tex", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, current->texture->GetTextureID());
	}
	glDrawArrays(GL_TRIANGLES, 0, 6);

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
	if (t != nullptr && t->GetJustification() == Justify::Unset) t->SetJustification(Justify::Centre);
	child->SetParent(this);
	float depth = realDepth + 0.1f;
	child->UpdateDepth(depth);
	return this;
}

void Button::LoadWithParameters(const std::map<String, String>& args)
{
	UIComponent::LoadWithParameters(args);

	if (args.find("NormalStyle") != args.end()) {
		std::vector<String> dats = split(args.at("NormalStyle"), ',');

		UIStyle s;

		switch (dats.size())
		{
		default:
		case 8: s.Tint.Z = (float)atof(dats[7].c_str());
		case 7: s.Tint.Y = (float)atof(dats[6].c_str());
		case 6: s.Tint.X = (float)atof(dats[5].c_str());
		case 5: s.texture = RI->LoadTextureByName(dats[4]);
		case 4: s.Opacity = (float)atof(dats[3].c_str());
		case 3: s.Color.Z = (float)atof(dats[2].c_str());
		case 2: s.Color.Y = (float)atof(dats[1].c_str());
		case 1: s.Color.X = (float)atof(dats[0].c_str());
		case 0: break;
		}

		SetStyle(s);
	}
	if (args.find("HoverStyle") != args.end()) {
		std::vector<String> dats = split(args.at("HoverStyle"), ',');

		UIStyle s;

		switch (dats.size())
		{
		default:
		case 8: s.Tint.Z = (float)atof(dats[7].c_str());
		case 7: s.Tint.Y = (float)atof(dats[6].c_str());
		case 6: s.Tint.X = (float)atof(dats[5].c_str());
		case 5: s.texture = RI->LoadTextureByName(dats[4]);
		case 4: s.Opacity = (float)atof(dats[3].c_str());
		case 3: s.Color.Z = (float)atof(dats[2].c_str());
		case 2: s.Color.Y = (float)atof(dats[1].c_str());
		case 1: s.Color.X = (float)atof(dats[0].c_str());
		case 0: break;
		}

		SetHoverStyle(s);
	}
	if (args.find("PressStyle") != args.end()) {
		std::vector<String> dats = split(args.at("PressStyle"), ',');

		UIStyle s;

		switch (dats.size())
		{
		default:
		case 8: s.Tint.Z = (float)atof(dats[7].c_str());
		case 7: s.Tint.Y = (float)atof(dats[6].c_str());
		case 6: s.Tint.X = (float)atof(dats[5].c_str());
		case 5: s.texture = RI->LoadTextureByName(dats[4]);
		case 4: s.Opacity = (float)atof(dats[3].c_str());
		case 3: s.Color.Z = (float)atof(dats[2].c_str());
		case 2: s.Color.Y = (float)atof(dats[1].c_str());
		case 1: s.Color.X = (float)atof(dats[0].c_str());
		case 0: break;
		}

		SetPressStyle(s);
	}
}

String Button::GetString() const
{
	String data("<Button ");
	data += UIComponent::GetString();
	data += " NormalStyle=\"" + style.Color.ToString() + ',' + std::to_string(style.Opacity) + ',' + (style.texture ? style.texture->GetName() : " ") + ',' + style.Tint.ToString() + '"';
	data += " HoverStyle=\"" + hovered.Color.ToString() + ',' + std::to_string(hovered.Opacity) + ',' + (hovered.texture ? hovered.texture->GetName() : " ") + ',' + hovered.Tint.ToString() + '"';
	data += " PressStyle=\"" + pressed.Color.ToString() + ',' + std::to_string(pressed.Opacity) + ',' + (pressed.texture ? pressed.texture->GetName() : " ") + ',' + pressed.Tint.ToString() + '"';
	if (child) {
		data += " >\n";
		data += '\t' + child->GetString();
		data += "</Button>\n";
	}
	else {
		data += " />\n";
	}

	return data;
}
