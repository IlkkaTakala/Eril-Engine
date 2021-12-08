#pragma once
#include <Core.h>
#include "../../OpenGL/UI/UIComponent.h"

class Shader;

struct UIStyle 
{
	UIStyle() {
		Color = Vector(1.f);
		Opacity = 1.f;
		texture = nullptr;
		Tint = Vector(1.f);
	}
	UIStyle(Vector color, float opacity = 1.f, Texture* tex = nullptr, Vector tint = Vector(1.f)) : Color(color), Opacity(opacity), texture(tex), Tint(tint) {}

	Vector Color;
	float Opacity;
	Texture* texture;
	Vector Tint;
};

class Image : public UIComponent
{
	REGISTER_UI(Image)
public:
	Image();
	virtual ~Image();

	virtual void Render() override;
	virtual void LoadWithParameters(const std::map<String, String>& args) override;

	Image* SetStyle(const UIStyle& s);

protected:
	Shader* solid_shader;
	Shader* texture_shader;
	uint uniformBuffer;

	UIStyle style;
};

