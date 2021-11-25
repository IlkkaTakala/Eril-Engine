#include <UI/Image.h>
#include <UI/Text.h>
#include <Interface/FileManager.h>

#include <glm/glm.hpp>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <Texture.h>
#include <Material.h>
#include "RenderCore/OpenGL/UI/UISpace.h"

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
	uint offset;
	uint letter;
};

struct Font
{
	Font() {
		tex = nullptr;
		kerningTexture = 0;
		charBuffer = 0;
		charCount = 0;
		kerningCount = 0;
	}
	~Font() {
	}
	void Destroy() {
		delete tex;
		glDeleteBuffers(1, &charBuffer);
		glDeleteTextures(1, &kerningTexture);
	}
	Texture* tex;
	int charCount;
	int kerningCount;
	Glyph glyphs[256];
	std::map<char, std::map<char, int>> kernings;
	uint charBuffer;
	uint kerningTexture;
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

	int* kernData = new int[256 * 256]();

	for (int i = 0; i < f.kerningCount; i++) {
		line = readLine(line, data);
		int first = atoi(readParamOnLine("first", line, data).c_str());
		int second = atoi(readParamOnLine("second", line, data).c_str());
		int kern = atoi(readParamOnLine("amount", line, data).c_str());
		int idx = first * 256 + second;
		if (idx < 256 * 256 && idx >= 0)
		{
			kernData[idx] = kern;
			f.kernings[first][second] = kern;
		}
		else Console::Error("Invalid kerning");
	}

	glGenTextures(1, &f.kerningTexture);
	glBindTexture(GL_TEXTURE_2D, f.kerningTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8I, 256, 256, 0, GL_RED_INTEGER, GL_INT, kernData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	glGenBuffers(1, &f.charBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, f.charBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Glyph) * 256, f.glyphs, GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	delete[] kernData;

	f.tex = RI->LoadTextureByName("Assets/Fonts/" + file);

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
	uint offset;
	uint letter;
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
uniform int fontsize;
layout (binding=0, rgba16f) uniform image2D colorDest;
layout (binding=4, r8) readonly uniform image2D fontAtlas;
layout (binding=5, r8i) readonly uniform iimage2D kerning;
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

shared uint epoch;

const float width = 0.3;
const float fade = 0.2;

#define TILE_SIZE 64
layout(local_size_x = TILE_SIZE, local_size_y = 1, local_size_z = 1) in;
void main() 
{
	uint character = 0;
	
	if (gl_LocalInvocationIndex == 0) {
		epoch = 0;
	}

	uint threadCount = TILE_SIZE;
    uint passCount = (stringBuffer.length() + threadCount - 1) / threadCount;
	for (uint passIt = 0; passIt < passCount; passIt++)
	{
		uint stringIndex =  passIt * threadCount + gl_LocalInvocationIndex;

		if (stringIndex >= stringBuffer.length()) break;

		uint letter = stringBuffer[stringIndex].letter;
		ivec2 size = charBuffer[letter].size;
		uint xoffset = topLeft.x + stringBuffer[stringIndex].offset + charBuffer[letter].offset.y;
		uint yoffset = topLeft.y + charBuffer[letter].offset.y;
		ivec2 dataLoc = charBuffer[letter].topLeft;
		vec4 result = vec4(1.0);
		for (int y = 0; y < size.y; y++) {
			for (int x = 0; x < size.x; x++) {
				ivec2 loc = ivec2(xoffset + x, screenSize.y - (yoffset + y));
				//if (texture(depthMap, loc / vec2(screenSize)).r == depthValue) {
					float fontValue = imageLoad(fontAtlas, dataLoc + ivec2(x, y)).r;
					result.a = smoothstep(width, width + fade, fontValue);
					vec4 ogColor = imageLoad(colorDest, loc);
					imageStore(colorDest, loc, vec4(ogColor.xyz * (1.0 - result.a) + result.xyz * result.a, ogColor.a + result.a));
				//}
			}
		}
	}
}
)~~~";

	solid_shader = new Shader(0, solidShader);

	SetStyle(style);

	hits = HitReg::HitTestVisible;

	font = "Assets/Fonts/arial";

	auto it = fonts.find(font);
	if (it == fonts.end()) fonts[font] = loadFont(font);

	fontSize = 12;

	SetText("Hello World");
}

void Text::Render()
{
	if (visible != Visibility::Visible) return;
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, fonts[font].charBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, StringBuffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, uniformBuffer);

	solid_shader->Bind();

	solid_shader->SetUniform("topLeft", topLeft.X, topLeft.Y);
	solid_shader->SetUniform("fontsize", fontSize);
	solid_shader->SetUniform("model", matrix->model_m);
	solid_shader->SetUniform("depthValue", realDepth);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, RI->GetUIManager()->GetDepth());

	glBindImageTexture(0, RI->GetUIManager()->GetColor(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
	glBindImageTexture(4, fonts[font].tex->GetTextureID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R8);
	glBindImageTexture(5, fonts[font].kerningTexture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R8I);

	glDispatchCompute(1, 1, 1);
	glMemoryBarrier(GL_FRAMEBUFFER_BARRIER_BIT);
	//glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Text::SetText(const String& text)
{
	value = text;
	Letter* storage = new Letter[value.size()]();
	for (int i = 0; i < value.size(); i++) {
		char letter = value[i];
		storage[i].letter = letter;
		if (i > 0) {
			storage[i].offset = storage[i - 1].offset + fonts[font].glyphs[storage[i - 1].letter].advance;
			if (fonts[font].kernings.count(storage[i - 1].letter) > 0) {
				if (fonts[font].kernings[storage[i - 1].letter].count(letter) > 0)
					storage[i].offset += (int)fonts[font].kernings[storage[i - 1].letter][letter];
			}
		}
		else {
			storage[i].offset = 0;
		}
	}

	glGenBuffers(1, &StringBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, StringBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Letter) * value.size(), storage, GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
