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

layout (std140, binding = 0) uniform Globals
{
    mat4 projection;
    mat4 view;
	vec4 viewPos;
	ivec2 screenSize;
	int sceneLightCount;
};

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

float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

vec3 WorldPosFromDepth(float depth) {
    float z = depth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(TexCoords * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = inverse(projection) * clipSpacePosition;

    // Perspective division
    viewSpacePosition /= viewSpacePosition.w;

    vec4 worldSpacePosition = inverse(view) * viewSpacePosition;

    return worldSpacePosition.xyz;
}

const float gamma = 2.2;
const float exposure = 0.6;
const float b = 0.1; // Density
const float a = 1.1; //Scatter


vec3 applyFog( in vec3  rgb,      // original color of the pixel
               in float distance, // camera to point distance
               in vec3  rayDir,   // camera to point vector
               in vec3  sunDir )  // sun light direction
{
    float fogAmount = 1.0 - exp( -distance*b );
    float sunAmount = max( dot( rayDir, sunDir ), 0.0 ) * 0.8;
    vec3  fogColor  = mix( vec3(0.1,0.2,0.32), // bluish
                           //vec3(1.0,0.9,0.7), // yellowish
                           vec3(1.0,1.0,1.0), // yellowish
                           pow(sunAmount,8.0) );
    return mix( rgb, fogColor, fogAmount );
}

vec3 applyFogLower( in vec3  rgb,      // original color of the pixel
               in float distance, // camera to point distance
               in vec3  rayOri,   // camera position
               in vec3  rayDir )  // camera to point vector
{
    float fogAmount = (a/b) * exp(-rayOri.y*b) * (1.0-exp( -distance*rayDir.y*b ))/rayDir.y;
    vec3  fogColor  = vec3(0.5,0.6,0.7);
    return mix( rgb, fogColor, fogAmount );
}

void main()
{
    vec3 hdrColor = texture(Color, TexCoords).rgb;      
    vec3 bloomColor = texture(Bloom, TexCoords).rgb;
    hdrColor += bloomColor;
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);  
    result = pow(result, vec3(1.0 / gamma));
	
	vec3 pixelPos = WorldPosFromDepth(texture(Depth, TexCoords).r);
	
	//result = applyFog(result, length(pixelPos - viewPos.xyz), normalize(pixelPos - viewPos.xyz), normalize(vec3(-1.0, 1.0, -1.0)));
	
    FragColor = vec4(result, 1.0);
}
###END_FRAGMENT###