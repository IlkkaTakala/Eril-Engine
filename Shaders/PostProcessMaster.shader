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

in vec2 TexCoords;
out vec4 FragColor;

void main()
{
	const float gamma = 2.2;
	const float exposure = 1.0;
    vec3 hdrColor = texture(Color, TexCoords).rgb;      
    vec3 bloomColor = texture(Bloom, TexCoords).rgb;
    hdrColor += bloomColor; // additive blending
    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0);
}
###END_FRAGMENT###