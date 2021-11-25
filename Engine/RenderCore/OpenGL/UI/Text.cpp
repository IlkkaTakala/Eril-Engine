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
	Glyph* glyphs = new Glyph[256]();
	for (int i = 0; i < f.charCount; i++) {
		line = readLine(line, data);
		int id = atoi(readParamOnLine("id", line, data).c_str());
		if (id < 0 || id > 255) {
			Console::Error("Invalid glyph found");
			continue;
		}
		glyphs[id].topLeft.x = atoi(readParamOnLine("x", line, data).c_str());
		glyphs[id].topLeft.y = atoi(readParamOnLine("y", line, data).c_str());
		glyphs[id].size.x = atoi(readParamOnLine("width", line, data).c_str());
		glyphs[id].size.y = atoi(readParamOnLine("height", line, data).c_str());
		glyphs[id].offset.x = atoi(readParamOnLine("xoffset", line, data).c_str());
		glyphs[id].offset.x = atoi(readParamOnLine("yoffset", line, data).c_str());
		glyphs[id].advance = atoi(readParamOnLine("xadvance", line, data).c_str());
		glyphs[id].page = atoi(readParamOnLine("page", line, data).c_str());
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
		if (idx < 256 * 256 && idx >= 0) kernData[idx] = kern;
		else Console::Error("Invalid kerning");
	}

	glGenTextures(1, &f.kerningTexture);
	glBindTexture(GL_TEXTURE_2D, f.kerningTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8I, 256, 256, 0, GL_RED_INTEGER, GL_INT, kernData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	glGenBuffers(1, &f.charBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, f.charBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Glyph) * 256, glyphs, GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	delete[] glyphs;
	delete[] kernData;

	f.tex = RI->LoadTextureByName("Assets/Fonts/" + file);

	auto end = std::chrono::steady_clock::now();
	duration = end - start;

	Console::Log("Loading font \"" + name + "\" took " + std::to_string(duration.count()));

	return f;
}

Text::Text()
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

struct CharData {
	ivec2 topLeft;
	ivec2 size;
	ivec2 offset;
	unsigned int advance;
	unsigned int page;
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

out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D depth;
uniform ivec2 size;
uniform int fontsize;
//layout (binding=0, rgba16f) writeonly uniform image2D colorDest;
layout (binding=4, r8) readonly uniform image2D fontAtlas;
layout (binding=5, r8i) readonly uniform iimage2D kerning;
layout (std140, binding = 1) uniform style
{
	vec4 color;
	vec4 tint;
};

layout(std430, binding = 2) readonly buffer StringBuffer {
	unsigned int data[];
} stringBuffer;

layout(std430, binding = 3) readonly buffer CharBuffer {
	CharData data[];
} charBuffer;

void main() 
{ 
	int advance = 0;
	uint char = 0;
	for (int i = 0; stringBuffer.data[i] != 0; i++) {
		char = stringBuffer.data[i];
		advance += charBuffer.data[char].advance;
		if (advance < TexCoords.x * size.x) break;
	}
	FragColor = imageLoad(fontAtlas, ivec2(size * TexCoords)) * color * tint; 

}
)~~~";

	solid_shader = new Shader(vertexShader, solidShader);

	SetStyle(style);

	hits = HitReg::HitTestVisible;

	font = "Assets/Fonts/arial";

	auto it = fonts.find(font);
	if (it == fonts.end()) fonts[font] = loadFont(font);

	text = "Hello World";
	fontSize = 12;

	glGenBuffers(1, &StringBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, StringBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(char) * text.size() + 1, (unsigned char*)text.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Text::Render()
{
	if (visible != Visibility::Visible) return;
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, fonts[font].charBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, StringBuffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, uniformBuffer);

	solid_shader->Bind();

	solid_shader->SetUniform("size", realSize.X, realSize.Y);
	solid_shader->SetUniform("fontsize", fontSize);
	solid_shader->SetUniform("model", matrix->model_m);

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, fonts[font].tex->GetTextureID());

	//glBindImageTexture(0, RI->GetUIManager()->GetColor(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
	glBindImageTexture(4, fonts[font].tex->GetTextureID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R8);
	glBindImageTexture(5, fonts[font].kerningTexture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R8I);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}
