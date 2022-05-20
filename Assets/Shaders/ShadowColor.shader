2;0;
###VERTEX###
#version 430 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_texCoord;
layout (location = 2) in vec3 in_normal;
layout (location = 3) in vec3 in_tangent;
layout (location = 4) in vec3 in_color;
layout (location = 5) in vec4 weights;
layout (location = 6) in ivec4 bones;

struct LightData {
	vec4 color;
	vec4 positionAndSize;
	vec4 rotation;
	ivec4 type;
	mat4 transform;
};

out vec2 TexCoords;
uniform mat4 lSpace;
uniform mat4 Model;

layout(std430, binding = 1) readonly buffer LightBuffer {
	LightData data[];
} lightBuffer;

layout(std430, binding = 3) readonly buffer BoneTransformsBuffer {
	mat4 data[];
} BoneTransforms;

void main()
{

	mat4 BoneTransform = BoneTransforms.data[bones[0]] * weights[0];
    BoneTransform += BoneTransforms.data[bones[1]] * weights[1];
    BoneTransform += BoneTransforms.data[bones[2]] * weights[2];
    BoneTransform += BoneTransforms.data[bones[3]] * weights[3];
		
    vec4 pos = Model * BoneTransform * vec4(in_position, 1.0);
	gl_Position = lSpace * pos;
}
###END_VERTEX###
###FRAGMENT###
#version 430 core

out float depth;

void main() 
{
	depth = gl_FragCoord.z;
}
###END_FRAGMENT###