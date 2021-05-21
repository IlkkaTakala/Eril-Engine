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
	vec4 position;
	float size;
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
};

layout(std430, binding = 1) readonly buffer LightBuffer {
	LightData data[];
} lightBuffer;

layout(std430, binding = 2) readonly buffer VisibleLightIndicesBuffer {
	VisibleIndex data[];
} visibleLightIndicesBuffer;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform int numberOfTilesX;
uniform int totalLightCount;
out vec4 FragColor;
  
in vec2 TexCoords;

float attenuate(vec3 lightDirection, float radius) {
	float cutoff = 0.5;
	float attenuation = dot(lightDirection, lightDirection) / (100.0 * radius);
	attenuation = 1.0 / (attenuation * 15.0 + 1.0);
	attenuation = (attenuation - cutoff) / (1.0 - cutoff);

	return clamp(attenuation, 0.0, 1.0);
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
	//normal = normalize(normal * 2.0 - 1.0);
	vec4 color = Albedo * 0.08;

	vec3 viewDirection = normalize(viewPos - FragPos).xyz;

	// The offset is this tile's position in the global array of valid light indices.
	// Loop through all these indices until we hit max number of lights or the end (indicated by an index of -1)
	// Calculate the lighting contribution from each visible point light
	uint offset = index * 1024;
	uint i;
	for (i = 0; i < 1024 && visibleLightIndicesBuffer.data[offset + i].index != -1; i++) {
		uint lightIndex = visibleLightIndicesBuffer.data[offset + i].index;
		LightData light = lightBuffer.data[lightIndex];

		vec4 lightColor = vec4(0.0, 0.0, 1.0, 1.0);
		float lightRadius = light.size;

		// Calculate the light attenuation on the pre-normalized lightDirection
		vec3 lightDirection = light.position.xyz - FragPos.xyz;
		//float attenuation = attenuate(lightDirection, lightRadius);

		// Normalize the light direction and calculate the halfway vector
		//lightDirection = normalize(lightDirection);
		//vec3 halfway = normalize(lightDirection + viewDirection);

		// Calculate the diffuse and specular components of the irradiance, then irradiance, and accumulate onto color
		vec4 diffuse = max(dot(normal, lightDirection), 0.0) * Albedo * lightColor;
		// How do I change the material propery for the spec exponent? is it the alpha of the spec texture?
		//float specular = pow(max(dot(normal, halfway), 0.0), 32.0);

		// Hacky fix to handle issue where specular light still effects scene once point light has passed into an object
		// if (diffuse == 0.0) {
			// specular = 0.0;
		// }

		//vec3 irradiance = lightColor.rgb * ((base_diffuse.rgb * diffuse));// + (base_specular.rgb * vec3(specular))) * attenuation;
		//color.rgb = vec3(diffuse);
		color += diffuse;
	}

	//color.rgb += base_diffuse.rgb * 0.08;

	// Use the mask to discard any fragments that are transparent
	//if (base_diffuse.a <= 0.2) {
	//	discard;
	//}
	vec4 result = vec4(1.0) - exp(-color * 1.0);

	// Minor gamma correction. Need to expand on it
	const float gamma = 2.2;
	result = pow(result, vec4(1.0 / gamma));
	
	float ratio = float(i) / float(totalLightCount);
	//FragColor = result;
	FragColor = vec4(ratio, ratio, color.b, 1.0);
}
###END_FRAGMENT###