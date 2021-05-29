3;2;
###VERTEX###
#version 430 core
layout (location = 0) in vec3 in_position;

void main()
{
	gl_Position = vec4(in_position, 1.0);
}
###END_VERTEX###
###GEOMETRY###
#version 430
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

out vec3 pos;

const mat3 V[6] = {
	mat3(vec3(0.0, 0.0,-1.0), vec3(0.0,-1.0, 0.0), vec3( 1.0, 0.0, 0.0)), 
	mat3(vec3(0.0, 0.0, 1.0), vec3(0.0,-1.0, 0.0), vec3(-1.0, 0.0, 0.0)),
	mat3(vec3(1.0, 0.0, 0.0), vec3(0.0, 0.0,-1.0), vec3(0.0,-1.0, 0.0)),
	mat3(vec3(1.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0), vec3(0.0, 1.0, 0.0)),
	mat3(vec3(1.0, 0.0, 0.0), vec3(0.0,-1.0, 0.0), vec3(0.0, 0.0, 1.0)),
	mat3(vec3(-1.0, 0.0, 0.0), vec3(0.0,-1.0, 0.0), vec3(0.0, 0.0,-1.0))
};

uniform mat4 P;

void main() {
    int i, layer;
	gl_Layer = 0;	
    for (layer = 0; layer < 6; layer++) {
        gl_Layer = layer;
        for (i = 0; i < 3; i++) {
            gl_Position = gl_in[i].gl_Position;
			pos = normalize(transpose(V[layer]) * (inverse(P) * gl_Position).xyz);
            EmitVertex();
        }
        EndPrimitive();
    }
}
###END_GEOMETRY###
###FRAGMENT###
#version 430 core

in vec3 pos;

layout (location = 0) out vec4 color;

uniform samplerCube env;
uniform int mip;

void main() 
{
	color = texture(env, pos);
}
###END_FRAGMENT###