#pragma once
#include "Core.h"
#include <list>
#include <glm/glm.hpp>
#include <glad/gl.h>

class VisibleObject;
class Material;
class Section;

class Shader
{
public:
	Shader(const char* const vertexShaderString, const char* const fragmentShaderString);
	~Shader();

	void Bind();

	void SetUniform(const String& name, const glm::mat4& m);

	void AddUser(Material*);
	void RemoveUser(Material*);

	std::list<Material*>& GetUsers();

private:
	GLint ShaderProgram;
	std::list<Material*> Users;
};

class Material
{
public:
	Material(Shader* s) : Shade(s) { s->AddUser(this); };
	~Material() { Shade->RemoveUser(this); }

	std::list<Section*>& GetObjects();

	void AddSection(Section*);
	void RemoveSection(Section*);

private:
	Shader* const Shade;

	std::list<Section*> Objects;
};


class Section
{
public:
	Section(Material* m) : Instance(m) {};
	~Section();
	Material* Instance;

private:
	GLuint VAO;
	GLuint Positions;
	GLuint Normals;
	GLuint UVs;

	long FaceCount;
};