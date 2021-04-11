#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "Objects/VisibleObject.h"
#include "Material.h"

Shader::Shader(const char* const vertexShaderString, const char* const fragmentShaderString) 
{
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderString, NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		printf("ERROR: Shader compilation failed: \"%s\"\n", infoLog);
	}

	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderString, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		printf("ERROR: Shader compilation failed: \"%s\"\n", infoLog);
	}

	ShaderProgram = glCreateProgram();
	glAttachShader(ShaderProgram, vertexShader);
	glAttachShader(ShaderProgram, fragmentShader);
	glLinkProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ShaderProgram, 512, NULL, infoLog);
		printf("ERROR: Shader link failed: \"%s\"\n", infoLog);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

Shader::~Shader()
{
	glDeleteProgram(ShaderProgram);
}

void Shader::Bind()
{
	glUseProgram(ShaderProgram);
}

void Shader::SetUniform(const String& name, const glm::mat4& m)
{
	GLint loc = glGetUniformLocation(ShaderProgram, name.c_str());
	if (loc < 0) return;
	glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
}

void Shader::AddUser(Material* m)
{
	Users.push_back(m);
}

void Shader::RemoveUser(Material* m)
{
	Users.remove(m);
}

std::list<Material*>& Shader::GetUsers()
{
	return Users;
}

std::list<Section*>& Material::GetObjects()
{
	return Objects;
}

void Material::AddSection(Section* o)
{
	Objects.push_back(o);
}

void Material::RemoveSection(Section* o)
{
	Objects.remove(o);
}

Section::~Section()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &Positions);
	glDeleteBuffers(1, &Normals);
	glDeleteBuffers(1, &UVs);
}
