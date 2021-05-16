#pragma once
#include "Core.h"
#include <list>
#include <map>
#include <glm/glm.hpp>
#include <glad/gl.h>

class VisibleObject;
class Material;
class Section;
class Texture;

class Shader
{
public:
	Shader(const char* const vertexShaderString, const char* const fragmentShaderString);
	~Shader();

	void Bind();

	void SetUniform(const String& name, const glm::mat4& m);
	void SetUniform(const String& name, const Vector& m);
	void SetUniform(const String& name, const float m);
	void SetUniform(const String& name, const int m);

	void AddUser(Material*);
	void RemoveUser(Material*);

	std::list<Material*>& GetUsers();

	int Success;

	uint8 Pass;

private:
	GLint ShaderProgram;
	std::list<Material*> Users;
};

class Material
{
public:
	Material(Shader* s) : Shade(s) { if (s != nullptr) s->AddUser(this); };
	~Material() { Shade->RemoveUser(this); }

	std::list<Section*>& GetObjects();
	std::map<String, Vector>& GetVectorParameters() { return VectorParameters; }
	std::map<String, float>& GetScalarParameters() { return ScalarParameters; }
	std::map<String, Texture*>& GetTextures() { return Textures; }

	void AddSection(Section*);
	void RemoveSection(Section*);

private:
	Shader* const Shade;

	std::map<String, Vector> VectorParameters;
	std::map<String, float> ScalarParameters;
	std::map<String, Texture*> Textures;
	std::list<Section*> Objects;
};

