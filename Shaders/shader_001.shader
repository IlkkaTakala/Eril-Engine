2;0;
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
layout (location = 0) out vec3 PositionBuffer;
layout (location = 1) out vec3 NormalBuffer;
layout (location = 2) out vec4 ColorBuffer;
layout (location = 3) out vec4 DataBuffer;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

void main()
{
	PositionBuffer = FragPos;
    NormalBuffer = normalize(Normal);
    ColorBuffer = vec4(1.0, 1.0, 1.0, 1.0);
}
###END_FRAGMENT###