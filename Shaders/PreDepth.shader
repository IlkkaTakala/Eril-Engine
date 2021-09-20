2;0;
###VERTEX###
#version 430 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_texCoord;
layout (location = 2) in vec3 in_normal;
layout (location = 3) in vec3 in_tangent;
layout (location = 4) in mat4 in_disp;

layout (std140, binding = 0) uniform Globals
{
    mat4 projection;
    mat4 view;
	vec4 viewPos;
	ivec2 screenSize;
	int sceneLightCount;
};

uniform mat4 Model;

void main()
{
	vec4 pos = Model * in_disp * vec4(in_position, 1.0);
	gl_Position = projection * view * pos;
}
###END_VERTEX###
###FRAGMENT###
#version 430 core

void main()
{
	
}
###END_FRAGMENT###