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

layout (binding = 0) uniform sampler2D Color;
layout (binding = 1) uniform sampler2D Bloom;
layout (binding = 4) uniform sampler2D Depth;

in vec2 TexCoords;
out vec4 FragColor;

float LinearizeDepth(float depth) {
	float z = depth * 2.0 - 1.0;
	return (2.0 * 0.1 * 1000.0) / (1000.0 + 0.1 - z * (1000.0 - 0.1));
}

vec3 blendNormal(vec3 base, vec3 blend, float opacity) {
	return (blend * opacity + base * (1.0 - opacity));
}

void main()
{
	const float gamma = 2.2;
	const float exposure = 1.0;
    vec3 hdrColor = texture(Color, TexCoords).rgb;      
    vec3 bloomColor = texture(Bloom, TexCoords).rgb;
    hdrColor += bloomColor;
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);  
    result = pow(result, vec3(1.0 / gamma));
	
	float depth = LinearizeDepth(texture(Depth, TexCoords).r) / 30.0;
	//result = blendNormal(result, vec3(0.5), clamp(depth - 0.15, 0.0, 8.0));

	
    FragColor = vec4(result, 1.0);
}
###END_FRAGMENT###