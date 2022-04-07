2;0;
###VERTEX###
#version 430 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_texCoord;

out vec2 TexCoords;

void main()
{
	TexCoords = in_texCoord;
	gl_Position = vec4(in_position, 1.0);
}
###END_VERTEX###
###FRAGMENT###
#version 430 core

layout (location = 1) out float SSAOBuffer;
layout (location = 0) out float SSAOBlurBuffer;

layout (std140, binding = 0) uniform Globals
{
    mat4 projection;
    mat4 view;
	vec4 viewPos;
	ivec2 screenSize;
	int sceneLightCount;
};

layout(std430, binding = 1) readonly buffer SSAOKernels {
	vec3 data[];
} samples;

layout (binding = 4) uniform sampler2D gDepth;
layout (binding = 5) uniform sampler2D gNormal;
layout (binding = 6) uniform sampler2D gPosition;
layout (binding = 7) uniform sampler2D texNoise;

in vec2 TexCoords;

vec4 WorldPosFromDepth(float depth) {
    float z = depth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(TexCoords * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = inverse(projection) * clipSpacePosition;

    // Perspective division
    viewSpacePosition /= viewSpacePosition.w;

    vec4 worldSpacePosition = inverse(view) * viewSpacePosition;

    return vec4(worldSpacePosition.xyz, 1.0);
}

void main()
{
	
	vec2 noiseScale = vec2(screenSize.x/4.0, screenSize.y/4.0);
	vec3 fragPos   = (view * texture(gPosition, TexCoords)).xyz;
	vec3 normal    = mat3(view) * normalize(texture(gNormal, TexCoords)).rgb;
	vec3 randomVec = texture(texNoise, TexCoords * noiseScale).xyz;
	
	vec3 tangent   = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN       = mat3(tangent, bitangent, normal);  
	
	int kernelSize = 64;
	float radius = 0.6;
	float bias = 0.025;
	
	float occlusion = 0.0;
	for(int i = 0; i < kernelSize; ++i)
	{
		// get sample position
		vec3 samplePos = TBN * samples.data[i]; // from tangent to view-space
		samplePos = fragPos + samplePos * radius; 
		
		vec4 offset = vec4(samplePos, 1.0);
		offset      = projection * offset;    // from view to clip-space
		offset.xyz /= offset.w;               // perspective divide
		offset.xyz  = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0  
		float sampleDepth = (view * texture(gPosition, offset.xy)).z; 
		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
	}
	occlusion = 1.0 - (occlusion / kernelSize);
	SSAOBuffer = texture(gNormal, TexCoords).w <= 0.0 ? 1.0 : occlusion;
	SSAOBlurBuffer = 0.0;

}
###END_FRAGMENT###