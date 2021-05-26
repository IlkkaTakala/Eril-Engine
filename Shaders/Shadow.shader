3;3;
###VERTEX###
#version 430 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;

void main()
{
    gl_Position = vec4(aPos, 1.0);
} 
###END_VERTEX###
###GEOMETRY###
#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

layout (std140, binding = 0) uniform Globals
{
    mat4 projection;
    mat4 view;
	vec4 viewPos;
	ivec2 screenSize;
	int sceneLightCount;
};

struct LightData {
	vec4 color;
	vec4 positionAndSize;
	vec4 rotation;
	ivec4 type;
	mat4 transforms[6];
};

layout(std430, binding = 1) readonly buffer LightBuffer {
	LightData data[];
} lightBuffer;

out vec4 FragPos; // FragPos from GS (output per emitvertex)
out vec3 lightPos;

void main()
{
		for(int face = 0; face < 6; ++face)
		{
			gl_Layer = face; // built-in variable that specifies to which face we render.
			lightPos = lightBuffer.data[0].positionAndSize.xyz;
			for(int i = 0; i < 3; ++i) // for each triangle vertex
			{
				FragPos = gl_in[i].gl_Position;
				gl_Position = lightBuffer.data[0].transforms[face] * FragPos;
				EmitVertex();
			}    
			EndPrimitive();
		}
} 
###END_GEOMETRY###
###FRAGMENT###
#version 430 core
in vec4 FragPos;

in vec3 lightPos;

void main()
{
    // get distance between fragment and light source
    float lightDistance = length(FragPos.xyz - lightPos);
    
    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / 100.0;
    
    // write this as modified depth
    gl_FragDepth = lightDistance;
}
###END_FRAGMENT###
