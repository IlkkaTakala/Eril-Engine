#include <UI/Image.h>
#include <UI/Text.h>
#include <Interface/FileManager.h>

#include <glm/glm.hpp>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <Texture.h>
#include <Material.h>
#include "RenderCore/OpenGL/UI/UISpace.h"

#include <UI/Panel.h>
#include <UI/Text.h>
#include <UI/TextBox.h>
#include <UI/Button.h>
#include <UI/VerticalPanel.h>

struct UIMatrix
{
	glm::mat4 model_m;
};

struct Glyph
{
	glm::ivec2 topLeft;
	glm::ivec2 size;
	glm::ivec2 offset;
	uint advance;
	uint page;
};

struct Letter
{
	glm::ivec2 offset;
	glm::ivec2 letter;
};

struct Font
{
	Font() {
		tex = nullptr;
		charBuffer = 0;
		charCount = 0;
		kerningCount = 0;
		originalSize = 0;
	}
	~Font() {
	}
	void Destroy() {
		delete tex;
		glDeleteBuffers(1, &charBuffer);
	}
	Texture* tex;
	int charCount;
	int kerningCount;
	Glyph glyphs[256];
	std::map<char, std::map<char, int>> kernings;
	uint charBuffer;
	uint originalSize;
};

static std::map<String, Font> fonts;

String readParamOnLine(const String& param, int off, String& data)
{
	String value;
	const char* str = data.c_str() + off;
	int count = 0;
	while (*str != '\n' && *str != '\0') {
		if (*str == param[0])
		{
			bool match = true;
			for (int i = 0; i < param.size() && match; i++) {
				match = *(str + i) == param[i];
			}
			if (match) {
				str += param.size() + 1;
				while (*str != ' ' && *str != '\n' && *str != '\0') {
					if (*str != '\"') value += *str;
					str++;
				}
				break;
			}
		}
		count++;
		str++;
	}
	return value;
}

int readLine(int off, String& data)
{
	const char* str = data.c_str() + off;
	int count = 0;
	while (true) {
		if (*str == '\n') {
			count++;
			break;
		}
		if (*str != '\0') {
			count++;
			str++;
		}
		else break;
		
	}
	if (count == 0) return 0;
	return count + off;
}

Font loadFont(String name)
{
	std::chrono::duration<float> duration = std::chrono::milliseconds(0);
	auto start = std::chrono::steady_clock::now();
	String data;
	FileManager::RequestData(name + ".fnt", data);
	std::stringstream stre(data);

	Font f;
	int line = 0;
	f.originalSize = atoi(readParamOnLine("size", line, data).c_str());
	for (int i = 0; i < 2; i++) {
		line = readLine(line, data);
	}
	String file = readParamOnLine("file", line, data);;
	line = readLine(line, data);
	f.charCount = atoi(readParamOnLine("count", line, data).c_str());
	//Glyph* glyphs = new Glyph[256]();
	for (int i = 0; i < f.charCount; i++) {
		line = readLine(line, data);
		int id = atoi(readParamOnLine("id", line, data).c_str());
		if (id < 0 || id > 255) {
			Console::Error("Invalid glyph found");
			continue;
		}
		f.glyphs[id].topLeft.x = atoi(readParamOnLine("x", line, data).c_str());
		f.glyphs[id].topLeft.y = atoi(readParamOnLine("y", line, data).c_str());
		f.glyphs[id].size.x = atoi(readParamOnLine("width", line, data).c_str());
		f.glyphs[id].size.y = atoi(readParamOnLine("height", line, data).c_str());
		f.glyphs[id].offset.x = atoi(readParamOnLine("xoffset", line, data).c_str());
		f.glyphs[id].offset.y = atoi(readParamOnLine("yoffset", line, data).c_str());
		f.glyphs[id].advance = atoi(readParamOnLine("xadvance", line, data).c_str());
		f.glyphs[id].page = atoi(readParamOnLine("page", line, data).c_str());
	}
	line = readLine(line, data);
	f.kerningCount = atoi(readParamOnLine("count", line, data).c_str());

	for (int i = 0; i < f.kerningCount; i++) {
		line = readLine(line, data);
		int first = atoi(readParamOnLine("first", line, data).c_str());
		int second = atoi(readParamOnLine("second", line, data).c_str());
		int kern = atoi(readParamOnLine("amount", line, data).c_str());
		int idx = first * 256 + second;
		if (idx < 256 * 256 && idx >= 0)
		{
			f.kernings[first][second] = kern;
		}
		else Console::Error("Invalid kerning");
	}

	glGenBuffers(1, &f.charBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, f.charBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Glyph) * 256, f.glyphs, GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	f.tex = IRender::LoadTextureByName("Assets/Fonts/" + file);

	auto end = std::chrono::steady_clock::now();
	duration = end - start;

	Console::Log("Loading font \"" + name + "\" took " + std::to_string(duration.count()));

	return f;
}

Text::Text()
{
	const char* solidShader = R"~~~(
#version 430 core

struct CharData {
	ivec2 topLeft;
	ivec2 size;
	ivec2 offset;
	unsigned int advance;
	unsigned int page;
};

struct LetterData {
	ivec2 offset;
	ivec2 letter;
};

// Shader storage buffer objects
layout (std140, binding = 0) uniform Globals
{
    mat4 projection;
    mat4 view;
	vec4 viewPos;
	ivec2 screenSize;
	int sceneLightCount;
};

uniform sampler2D depthMap;
uniform float depthValue;
uniform ivec2 topLeft;
uniform float fontsize;
uniform int weight;
layout (binding=0, rgba16f) uniform image2D colorDest;
layout (binding=4, r8) readonly uniform image2D fontAtlas;
layout (std140, binding = 1) uniform style
{
	vec4 color;
	vec4 tint;
};

layout(std430, binding = 2) readonly buffer StringBuffer {
	LetterData stringBuffer[];
};

layout(std430, binding = 3) readonly buffer CharBuffer {
	CharData charBuffer[];
};

shared float width;
const float fade = 0.25;

#define TILE_SIZE 64
layout(local_size_x = TILE_SIZE, local_size_y = 1, local_size_z = 1) in;
void main() 
{
	uint character = 0;
	
	if (gl_LocalInvocationIndex == 0) {
		width = 0.42 - (weight / 100.0 - 1.0) * 0.13;
	}

	uint threadCount = TILE_SIZE;
    uint passCount = (stringBuffer.length() + threadCount - 1) / threadCount;
	for (uint passIt = 0; passIt < passCount; passIt++)
	{
		uint stringIndex =  passIt * threadCount + gl_LocalInvocationIndex;

		if (stringIndex >= stringBuffer.length()) break;

		uint letter = stringBuffer[stringIndex].letter.x;
		ivec2 size = charBuffer[letter].size;
		int xoffset = stringBuffer[stringIndex].offset.x + charBuffer[letter].offset.x;
		xoffset = int(xoffset * fontsize);
		int yoffset = stringBuffer[stringIndex].offset.y + int(charBuffer[letter].offset.y * fontsize);
		ivec2 dataLoc = charBuffer[letter].topLeft;
		ivec2 endSize = ivec2(size * fontsize);
		for (int y = 0; y < endSize.y; y++) {
			for (int x = 0; x < endSize.x; x++) {
				vec4 result = vec4(1.0);
				vec2 screenLoc = vec2(xoffset + x, yoffset + y);
				ivec2 loc = ivec2(screenLoc.x + topLeft.x, screenSize.y - (topLeft.y + screenLoc.y));
				float fontValue = imageLoad(fontAtlas, dataLoc + ivec2((ivec2(x, y) / fontsize))).r;
				result.a = smoothstep(width, width + fade, fontValue);
				result *= color * tint;
				vec4 ogColor = imageLoad(colorDest, loc);
				imageStore(colorDest, loc, vec4(ogColor.xyz * (1.0 - result.a) + result.xyz * result.a, ogColor.a + result.a));
			}
		}
	}
}
)~~~";

	if (solid_shader != nullptr) delete solid_shader;
	solid_shader = new Shader(0, solidShader);

	hits = HitReg::HitTestInvisible;

	SetFont("Assets/Fonts/arial");

	SetStyle(Vector(0.f));

	fontSize = 30;
	weight = 100;
	just = Justify::Unset;

	SetText("Hello World");
}

Text::~Text()
{

}

void Text::Render()
{
	if (visible != Visibility::Visible) return;

	if (textChanged) {
		float ratio = fontSize / (float)fonts[font].originalSize;
		Letter* storage = new Letter[value.size()]();
		for (int i = 0; i < value.size(); i++) {
			char letter = value[i];
			storage[i].letter.x = letter;
			if (i > 0) {
				storage[i].offset.x = storage[i - 1].offset.x + uint(fonts[font].glyphs[storage[i - 1].letter.x].advance - 20);
				if (fonts[font].kernings.count(storage[i - 1].letter.x) > 0) {
					if (fonts[font].kernings[storage[i - 1].letter.x].count(letter) > 0)
						storage[i].offset += (int)(fonts[font].kernings[storage[i - 1].letter.x][letter]);
				}
				storage[i].offset.y = 0;
			}
			else {
				storage[i].offset.x = 0;
				storage[i].offset.y = 0;
			}
		}
		int total_length = value == "" ? 0 : storage[value.size() - 1].offset.x + fonts[font].glyphs[storage[value.size() - 1].letter.x].advance;
		int center = (int)(realSize.X - total_length * ratio) / 2;
		int total_height = realSize.Y - fontSize;

		switch (just)
		{
		case Justify::Unset:
		case Justify::Left:
			break;
		case Justify::Centre:
			for (int i = 0; i < value.size(); i++) {
				storage[i].offset.x += int(center / ratio);
				storage[i].offset.y += total_height / 2;
			}
			break;
		case Justify::Right:
			for (int i = 0; i < value.size(); i++) {
				storage[i].offset.x -= total_length;
			}
			break;
		default:
			break;
		}

		glGenBuffers(1, &StringBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, StringBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Letter) * value.size(), storage, GL_STATIC_DRAW);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		delete[] storage;

		textChanged = false;
	}

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, fonts[font].charBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, StringBuffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, uniformBuffer);

	solid_shader->Bind();

	solid_shader->SetUniform("topLeft", topLeft.X, topLeft.Y);
	solid_shader->SetUniform("fontsize", fontSize / (float)fonts[font].originalSize);
	solid_shader->SetUniform("model", matrix->model_m);
	solid_shader->SetUniform("depthValue", realDepth);
	solid_shader->SetUniform("weight", weight);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, IRender::GetUIManager()->GetDepth());

	glBindImageTexture(0, IRender::GetUIManager()->GetColor(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
	glBindImageTexture(4, fonts[font].tex->GetTextureID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R8);

	glDispatchCompute(1, 1, 1);
	glMemoryBarrier(GL_FRAMEBUFFER_BARRIER_BIT);
	//glDrawArrays(GL_TRIANGLES, 0, 6);
}

Text* Text::SetText(const String& text, int size)
{
	value = text;
	textChanged = true;
	if (size != 0) fontSize = size;

	return this;
}

Text* Text::SetFont(const String& name)
{
	if (name == "") return this;
	font = name;
	auto it = fonts.find(font);
	if (it == fonts.end()) fonts[font] = loadFont(font);

	return this;
}

void Text::LoadWithParameters(const std::map<String, String>& args)
{
	Image::LoadWithParameters(args);

	if (args.find("Text") != args.end()) {
		String dats = args.at("Text");
		SetText(dats);
	}
	if (args.find("TextStyle") != args.end()) {
		std::vector<String> dats = split(args.at("TextStyle"), ',');

		UIStyle s;

		switch (dats.size())
		{
		default:
		case 3: SetJustification((Justify)atoi(dats[2].c_str()));
		case 2: SetFont(dats[1]);
		case 1: fontSize = (int)atof(dats[0].c_str());
		case 0: break;
		}

		textChanged = true;
	}
}

String Text::GetString() const
{
	String data("<Text ");
	data += UIComponent::GetString();
	data += " Style=\"" + style.Color.ToString() + ',' + std::to_string(style.Opacity) + ',' + (style.texture ? style.texture->GetName() : " ") + ',' + style.Tint.ToString() + '"';
	data += " Text=\"" + value + "\"";
	data += " TextStyle=\"" + std::to_string(fontSize) + ',' + font + ',' + std::to_string((uint8)just) + "\"";
	data += " />\n";
	return data;
}

void Text::MakeEditMenu(std::vector<UIComponent*>& comps)
{
	Image::MakeEditMenu(comps);
	VerticalPanel* topLevel = new VerticalPanel();
	topLevel->SetTransform(0.f, 0.f, 0.f, 200.f, Vector(0.f), Vector(0.f, 1.f, 0.f));
	TextBox* value_box = new TextBox();
	TextBox* font_box = new TextBox();
	TextBox* size_box = new TextBox();
	TextBox* just_box = new TextBox();

	topLevel->AddChild(
		(new Panel())->AddChild(
			(new Text())->SetText("Text: ", 20)->SetTransform()
		)->AddChild(
			value_box->SetText(value)->SetFontSize(20)->SetTransform(0.f, 300.f, 0.f, 30.f, Vector(0.f), Vector(0.25f))
		)->SetTransform(0.f, 0.f, 0.f, 30.f)
	)->AddChild(
		(new Panel())->AddChild(
			(new Text())->SetText("Fontsize: ", 20)->SetTransform()
		)->AddChild(
			size_box->SetText(std::to_string(fontSize))->SetFontSize(20)->SetTransform(0.f, 300.f, 0.f, 30.f, Vector(0.f), Vector(0.25f))
		)->SetTransform(0.f, 0.f, 0.f, 30.f)
	)->AddChild(
		(new Panel())->AddChild(
			(new Text())->SetText("Font: ", 20)->SetTransform()
		)->AddChild(
			font_box->SetText(font)->SetFontSize(20)->SetTransform(0.f, 300.f, 0.f, 30.f, Vector(0.f), Vector(0.25f))
		)->SetTransform(0.f, 0.f, 0.f, 30.f)
	)->AddChild(
		(new Panel())->AddChild(
			(new Text())->SetText("Justify: ", 20)->SetTransform()
		)->AddChild(
			just_box->SetText(std::to_string((uint8)just))->SetFontSize(20)->SetTransform(0.f, 300.f, 0.f, 30.f, Vector(0.f), Vector(0.25f))
		)->SetTransform(0.f, 0.f, 0.f, 30.f)
	)->AddChild(
		(new Panel())->SetTransform(0.f, 0.f, 0.f, 10.f)
	)->AddChild(
		(new Button())->AddChild((new Text())->SetText("Apply", 20))->SetTransform(0.f, 0.f, 0.f, 30.f)
		->SetEventCallback(Constants::UI::UI_ON_MOUSE_DOWN, [this, value_box, size_box, font_box, just_box]() {
		std::map<String, String> data;
		String trans = size_box->GetText() + ',' + font_box->GetText() + ',' + just_box->GetText();
		data.emplace("Text", value_box->GetText());
		data.emplace("TextStyle", trans);
		LoadWithParameters(data);
	})
	);

	comps.push_back(topLevel);
}


void Text::UpdateMatrices(const Vector2D& size)
{
	Image::UpdateMatrices(size);

	textChanged = true;
}
