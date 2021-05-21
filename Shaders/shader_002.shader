2;1;
###VERTEX###
#version 430 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_texCoord;
layout (location = 2) in vec3 in_normal;

layout (std140, binding = 0) uniform Globals
{
    mat4 projection;
    mat4 view;
};

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

void main()
{
	gl_Position = projection * view * vec4(in_position.x, in_position.y, in_position.z, 1.0);
	FragPos = vec3(in_position.x, in_position.y, in_position.z);
	TexCoords = in_texCoord;
	Normal = in_normal;
}
###END_VERTEX###
###FRAGMENT###
#version 430 core

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0, 1.0, 0.0, 0.5);
}
###END_FRAGMENT###