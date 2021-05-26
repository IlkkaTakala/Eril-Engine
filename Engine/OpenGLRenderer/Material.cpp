#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "Objects/VisibleObject.h"
#include "Material.h"

Shader::Shader(const char* const vertexShaderString, const char* const fragmentShaderString) 
{
	Success = false;
	Pass = 0;

	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderString, NULL);
	glCompileShader(vertexShader);

	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &Success);
	if (!Success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		printf("ERROR: Shader compilation failed: \"%s\"\n", infoLog);
		return;
	}

	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderString, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &Success);
	if (!Success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		printf("ERROR: Shader compilation failed: \"%s\"\n", infoLog);
		return;
	}

	ShaderProgram = glCreateProgram();
	glAttachShader(ShaderProgram, vertexShader);
	glAttachShader(ShaderProgram, fragmentShader);
	glLinkProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, 512, NULL, infoLog);
		printf("ERROR: Shader link failed: \"%s\"\n", infoLog);
		return;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

Shader::Shader(const char* const vertexShaderString, const char* geomShaderString, const char* const fragmentShaderString)
{
	Success = false;
	Pass = 0;

	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderString, NULL);
	glCompileShader(vertexShader);

	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &Success);
	if (!Success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		printf("ERROR: Shader compilation failed: \"%s\"\n", infoLog);
		return;
	}

	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderString, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &Success);
	if (!Success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		printf("ERROR: Shader compilation failed: \"%s\"\n", infoLog);
		return;
	}

	int geomShader = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geomShader, 1, &geomShaderString, NULL);
	glCompileShader(geomShader);
	glGetShaderiv(geomShader, GL_COMPILE_STATUS, &Success);
	if (!Success) {
		glGetShaderInfoLog(geomShader, 512, NULL, infoLog);
		printf("ERROR: Shader compilation failed: \"%s\"\n", infoLog);
		return;
	}

	ShaderProgram = glCreateProgram();
	glAttachShader(ShaderProgram, vertexShader);
	glAttachShader(ShaderProgram, fragmentShader);
	glAttachShader(ShaderProgram, geomShader);
	glLinkProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, 512, NULL, infoLog);
		printf("ERROR: Shader link failed: \"%s\"\n", infoLog);
		return;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(geomShader);
}

Shader::Shader(int type, const char* const ShaderString)
{
	char infoLog[512];
	switch (type)
	{
	case 0:
	{
		GLuint compute;
		Pass = 3;
		// Compute shader
		compute = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(compute, 1, &ShaderString, NULL);
		glCompileShader(compute);
		glGetShaderiv(compute, GL_COMPILE_STATUS, &Success);
		if (!Success) {
			glGetShaderInfoLog(compute, 512, NULL, infoLog);
			printf("ERROR: Shader compile failed: \"%s\"\n", infoLog);
			return;
		}
		// Create the shader program
		ShaderProgram = glCreateProgram();
		glAttachShader(ShaderProgram, compute);
		glLinkProgram(ShaderProgram);

		glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
		if (!Success) {
			glGetProgramInfoLog(ShaderProgram, 512, NULL, infoLog);
			printf("ERROR: Shader link failed: \"%s\"\n", infoLog);
			return;
		}
		// No longer need the shaders, delete them
		glDeleteShader(compute);
	} break;

	case 1:
	{
		int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &ShaderString, NULL);
		glCompileShader(vertexShader);

		char infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &Success);
		if (!Success) {
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			printf("ERROR: Shader compilation failed: \"%s\"\n", infoLog);
			return;
		}

		ShaderProgram = glCreateProgram();
		glAttachShader(ShaderProgram, vertexShader);
		glLinkProgram(ShaderProgram);
		glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
		if (!Success) {
			glGetProgramInfoLog(ShaderProgram, 512, NULL, infoLog);
			printf("ERROR: Shader link failed: \"%s\"\n", infoLog);
			return;
		}

		glDeleteShader(vertexShader);
	} break;

	case 2:
	{
		int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &ShaderString, NULL);
		glCompileShader(fragmentShader);
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &Success);
		if (!Success) {
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			printf("ERROR: Shader compilation failed: \"%s\"\n", infoLog);
			return;
		}

		ShaderProgram = glCreateProgram();
		glAttachShader(ShaderProgram, fragmentShader);
		glLinkProgram(ShaderProgram);
		glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
		if (!Success) {
			glGetProgramInfoLog(ShaderProgram, 512, NULL, infoLog);
			printf("ERROR: Shader link failed: \"%s\"\n", infoLog);
			return;
		}

		glDeleteShader(fragmentShader);
	} break;
	}
	
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

void Shader::SetUniform(const String& name, const Vector& m)
{
	GLint loc = glGetUniformLocation(ShaderProgram, name.c_str());
	if (loc < 0) return;
	glUniform3fv(loc, 3, &m[0]);
}

void Shader::SetUniform(const String& name, const int x, const int y)
{
	GLint loc = glGetUniformLocation(ShaderProgram, name.c_str());
	if (loc < 0) return;
	glUniform2iv(loc, 1, &glm::ivec2(x, y)[0]);
}

void Shader::SetUniform(const String& name, const float m)
{
	GLint loc = glGetUniformLocation(ShaderProgram, name.c_str());
	if (loc < 0) return;
	glUniform1fv(loc, 1, &m);
}

void Shader::SetUniform(const String& name, const int m)
{
	GLint loc = glGetUniformLocation(ShaderProgram, name.c_str());
	if (loc < 0) return;
	glUniform1i(loc, m);
}

void Shader::SetUniform(const String& name, const uint m)
{
	GLint loc = glGetUniformLocation(ShaderProgram, name.c_str());
	if (loc < 0) return;
	glUniform1ui(loc, m);
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
