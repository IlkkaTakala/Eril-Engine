2;0;
###VERTEX###
#version 430 core
layout (location = 0) in vec3 in_position;

out vec2 TexCoords;
uniform mat4 lSpace;

void main()
{
	gl_Position = lSpace * vec4(in_position, 1.0);
}
###END_VERTEX###
###FRAGMENT###
#version 430 core

void main() 
{
	
}
###END_FRAGMENT###