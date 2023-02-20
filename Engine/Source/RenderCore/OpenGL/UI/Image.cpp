#include <UI/Image.h>
#include "Material.h"
#include "Texture.h"

#include <UI/Panel.h>
#include <UI/Text.h>
#include <UI/TextBox.h>
#include <UI/Button.h>
#include <UI/VerticalPanel.h>

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
	vec4 res = texture(tex, TexCoords) * tint * color;
	res.a *= color.a;
	FragColor = res; 
}
)~~~";

	solid_shader = new Shader(vertexShader, solidShader);
	texture_shader = new Shader(vertexShader, textureShader);

	UIStyleGLM n(style);
	glGenBuffers(1, &uniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(UIStyleGLM), &n, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	hits = HitReg::HitTestVisible;
}

Image::~Image()
{
	delete solid_shader;
	delete texture_shader;

	glDeleteBuffers(1, &uniformBuffer);
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

void Image::LoadWithParameters(const std::map<String, String>& args)
{
	UIComponent::LoadWithParameters(args);

	if (args.find("Style") != args.end()) {
		std::vector<String> dats = split(args.at("Style"), ',');

		UIStyle s;

		switch (dats.size())
		{
		default:
		case 8: s.Tint.Z = (float)atof(dats[7].c_str());
		case 7: s.Tint.Y = (float)atof(dats[6].c_str());
		case 6: s.Tint.X = (float)atof(dats[5].c_str());
		case 5: s.texture = IRender::LoadTextureByName(dats[4]);
		case 4: s.Opacity = (float)atof(dats[3].c_str());
		case 3: s.Color.Z = (float)atof(dats[2].c_str());
		case 2: s.Color.Y = (float)atof(dats[1].c_str());
		case 1: s.Color.X = (float)atof(dats[0].c_str());
		case 0: break;
		}

		SetStyle(s);
	}
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

String Image::GetString() const
{
	String data("<Image ");
	data += UIComponent::GetString();
	data += " Style=\"" + style.Color.ToString() + ',' + std::to_string(style.Opacity) + ',' + (style.texture ? style.texture->GetName() : " ") + ',' + style.Tint.ToString() + '"';
	data += " />\n";
	return data;
}

void Image::MakeEditMenu(std::vector<UIComponent*>& comps)
{
	UIComponent::MakeEditMenu(comps);

	VerticalPanel* topLevel = new VerticalPanel();
	topLevel->SetTransform(0.f, 0.f, 0.f, 150.f, Vector(0.f), Vector(0.f, 1.f, 0.f));
	TextBox* offset_box = new TextBox();
	TextBox* origin_box = new TextBox();
	TextBox* anchor_box = new TextBox();


	topLevel->AddChild(
		(new Panel())->AddChild(
			(new Text())->SetText("Color: ", 20)->SetTransform()
		)->AddChild(
			offset_box->SetText(style.Color.ToString() + ',' + std::to_string(style.Opacity))->SetFontSize(20)->SetTransform(0.f, 300.f, 0.f, 30.f, Vector(0.f), Vector(0.25f))
		)->SetTransform(0.f, 0.f, 0.f, 30.f)
	)->AddChild(
		(new Panel())->AddChild(
			(new Text())->SetText("Texture: ", 20)->SetTransform()
		)->AddChild(
			anchor_box->SetText(style.texture ? style.texture->GetName() : "")->SetFontSize(20)->SetTransform(0.f, 300.f, 0.f, 30.f, Vector(0.f), Vector(0.25f))
		)->SetTransform(0.f, 0.f, 0.f, 30.f)
	)->AddChild(
		(new Panel())->AddChild(
			(new Text())->SetText("Tint: ", 20)->SetTransform()
		)->AddChild(
			origin_box->SetText(style.Tint.ToString())->SetFontSize(20)->SetTransform(0.f, 300.f, 0.f, 30.f, Vector(0.f), Vector(0.25f))
		)->SetTransform(0.f, 0.f, 0.f, 30.f)
	)->AddChild(
		(new Panel())->SetTransform(0.f, 0.f, 0.f, 10.f)
	)->AddChild(
		(new Button())->AddChild((new Text())->SetText("Apply", 20))->SetTransform(0.f, 0.f, 0.f, 30.f)
		->SetEventCallback(Constants::UI::UI_ON_MOUSE_DOWN, [this, offset_box, anchor_box, origin_box]() {
		std::map<String, String> data;
		String trans = offset_box->GetText() + ',' + anchor_box->GetText() + ',' + origin_box->GetText();
		data.emplace("Style", trans);
		LoadWithParameters(data);
	})
	);

	comps.push_back(topLevel);
}
