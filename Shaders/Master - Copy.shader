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
	int type;
	float intensity;
	float size;
	float distance;
	float falloff;
	
	vec3 color;
	vec3 position;
	vec3 rotation;
};

struct VisibleIndex {
	int index;
};

// Shader storage buffer objects
layout(std430, binding = 0) readonly buffer LightBuffer {
	LightData data[];
} lightBuffer;

layout(std430, binding = 1) readonly buffer VisibleLightIndicesBuffer {
	VisibleIndex data[];
} visibleLightIndicesBuffer;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform vec3 ViewPos;
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

void main()
{     
	// Determine which tile this pixel belongs to
	ivec2 location = ivec2(gl_FragCoord.xy);
	ivec2 tileID = location / ivec2(16, 16);
	uint index = tileID.y * numberOfTilesX + tileID.x;        
	
    // retrieve data from G-buffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec4 base_diffuse = texture(gAlbedoSpec, TexCoords);
	//vec4 base_specular = texture(texture_specular1, fragment_in.textureCoordinates);
	Normal = normalize(Normal * 2.0 - 1.0);
	vec4 color = vec4(0.0, 0.0, 0.0, 1.0);

	vec3 viewDirection = FragPos - ViewPos;//normalize(fragment_in.tangentViewPosition - fragment_in.tangentFragmentPosition);

	// The offset is this tile's position in the global array of valid light indices.
	// Loop through all these indices until we hit max number of lights or the end (indicated by an index of -1)
	// Calculate the lighting contribution from each visible point light
	uint offset = index * 1024;
	for (uint i = 0; i < 1024 && visibleLightIndicesBuffer.data[offset + i].index != -1; i++) {
		uint lightIndex = visibleLightIndicesBuffer.data[offset + i].index;
		LightData light = lightBuffer.data[lightIndex];

		vec4 lightColor = vec4(light.color, 1.0);
		vec3 tangentLightPosition = light.position;
		float lightRadius = light.size;

		// Calculate the light attenuation on the pre-normalized lightDirection
		vec3 lightDirection = tangentLightPosition - FragPos;//fragment_in.tangentFragmentPosition;
		float attenuation = attenuate(lightDirection, lightRadius);

		// Normalize the light direction and calculate the halfway vector
		lightDirection = normalize(lightDirection);
		vec3 halfway = normalize(lightDirection + viewDirection);

		// Calculate the diffuse and specular components of the irradiance, then irradiance, and accumulate onto color
		float diffuse = max(dot(lightDirection, Normal), 0.0);
		// How do I change the material propery for the spec exponent? is it the alpha of the spec texture?
		float specular = pow(max(dot(Normal, halfway), 0.0), 32.0);

		// Hacky fix to handle issue where specular light still effects scene once point light has passed into an object
		if (diffuse == 0.0) {
			specular = 0.0;
		}

		vec3 irradiance = vec3(1.0);//lightColor.rgb * ((base_diffuse.rgb * diffuse) + (vec3(1.0, 1.0, 1.0) * vec3(specular)));// * attenuation;
		color.rgb += irradiance;
	}

	//color.rgb += base_diffuse.rgb * 0.08;

	// Use the mask to discard any fragments that are transparent
	//if (base_diffuse.a <= 0.2) {
		//discard;
	//}
	
	FragColor = color;
}
###END_FRAGMENT###