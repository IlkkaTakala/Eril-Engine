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

struct LightData {
	vec4 color;
	vec4 positionAndSize;
	vec4 rotation;
	ivec4 type;
	mat4 transforms[6];
};

struct VisibleIndex {
	int index;
};

// Shader storage buffer objects
layout (std140, binding = 0) uniform Globals
{
    mat4 projection;
    mat4 view;
	vec4 viewPos;
	ivec2 screenSize;
	int sceneLightCount;
};

layout(std430, binding = 1) readonly buffer LightBuffer {
	LightData data[];
} lightBuffer;

layout(std430, binding = 2) readonly buffer VisibleLightIndicesBuffer {
	VisibleIndex data[];
} visibleLightIndicesBuffer;

layout (location = 0) out vec4 ColorBuffer;
layout (location = 1) out vec4 BloomBuffer;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gDepth;
uniform sampler2D gData;
uniform sampler2D gSSAO;
uniform samplerCube gShadow;
uniform int numberOfTilesX;
out vec4 FragColor;
  
in vec2 TexCoords;

float attenuate(vec3 lightDirection, float radius) {
	float cutoff = 0.5;
	float attenuation = dot(lightDirection, lightDirection) / (100.0 * radius);
	attenuation = 1.0 / (attenuation * 15.0 + 1.0);
	attenuation = (attenuation - cutoff) / (1.0 - cutoff);

	return clamp(attenuation, 0.0, 1.0);
}

float LinearizeDepth(float depth) {
	float z = depth * 2.0 - 1.0;
	return (2.0 * 0.1 * 100.0) / (100.0 + 0.1 - z * (100.0 - 0.1));
}

void main()
{     
	ivec2 location = ivec2(gl_FragCoord.xy);
	ivec2 tileID = location / ivec2(16, 16);
	uint index = tileID.y * numberOfTilesX + tileID.x;

	// Get color and normal components from texture maps
	vec4 Albedo = texture(gAlbedoSpec, TexCoords);
	vec4 FragPos = texture(gPosition, TexCoords);
	vec3 normal = texture(gNormal, TexCoords).rgb;
	vec4 data = texture(gData, TexCoords);
	float Ambient = texture(gSSAO, TexCoords).r;
	vec4 color = Albedo * 0.1 * Ambient * data.b;

	vec3 viewDirection = normalize(viewPos - FragPos).xyz;

	uint offset = index * 1024;
	uint i;
	float closestDepth = 0.0;
	for (i = 0; i < 1024 && visibleLightIndicesBuffer.data[offset + i].index != -1; i++) {
		uint lightIndex = visibleLightIndicesBuffer.data[offset + i].index;
		LightData light = lightBuffer.data[lightIndex];

		

		switch (light.type.x)
		{
			case 0:
			{
				closestDepth = texture(gShadow, light.rotation.xyz).r;
				vec3 direction = normalize(-light.rotation.xyz);
				float diffuseFactor = max(dot(direction, normal), 0.0f);
				color += vec4(light.color.xyz, 0.0f) * Albedo * diffuseFactor * 4.0;
			} break;
			
			case 1:
			{
				vec3 direction = light.positionAndSize.xyz - FragPos.xyz;
				vec3 fragToLight = FragPos.xyz - light.positionAndSize.xyz;
				// use the light to fragment vector to sample from the depth map    
				closestDepth = texture(gShadow, fragToLight).r;
				if(length(direction) <= light.positionAndSize.w) {
					float attenuation = 1.0f - length(direction) / (light.positionAndSize.w);
					//float attenuation = attenuate(direction, light.size.w);
					direction = normalize(direction);
					float diffuseFactor = max(dot(direction, normal), 0.0f);
					color += vec4(light.color.xyz, 0.0f) * Albedo * diffuseFactor * 1.0 * attenuation;
				}
			} break;
		}
		
	}
	
	// Height fog
	//float depth = LinearizeDepth(texture(gDepth, TexCoords).r) / 100.0;
	//color += clamp(depth * 2 - 0.5, 0.0, 8.0);
	
	const float gamma = 2.2;
	const float exposure = 1.0;
	
	vec4 result = vec4(1.0) - exp(-color * exposure);
    // also gamma correct while we're at it       
    result = pow(result, vec4(1.0 / gamma));
	
	ColorBuffer = result;
	//if (brightness > 1.0)
	BloomBuffer = clamp(result - 1.0, 0.0, 100.0);
}
###END_FRAGMENT###