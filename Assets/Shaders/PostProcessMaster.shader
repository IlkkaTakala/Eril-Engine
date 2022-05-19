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
layout (binding = 5) uniform sampler2D Normal;
layout (binding = 6) uniform sampler2D Position;
layout (binding = 7) uniform sampler2D Data;
layout (binding = 9) uniform sampler2D SSAO;

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

const float rayStep = 0.1;
const float minRayStep = 0.1;
const int maxSteps = 30;
const int numBinarySearchSteps = 5;
const float reflectionSpecularFalloffExponent = 3.0;

#define scale vec3(.8, .8, .8)
#define K 19.19

vec3 binarySearch(inout vec3 dir, inout vec3 hitCoord, inout float dDepth) {
	float depth;
	vec4 projectedCoord;
	
	for(int i = 0; i < numBinarySearchSteps; i++){
		projectedCoord = projection * vec4(hitCoord, 1.0);
		projectedCoord.xy /= projectedCoord.w;
		projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
		
		depth = (view * texture(Position, projectedCoord.xy)).z;
		dDepth = hitCoord.z - depth;
		dir *= 0.5;
		hitCoord += (dDepth > 0.0) ? dir : -dir;
	}
	
	projectedCoord = projection * vec4(hitCoord, 1.0);
	projectedCoord.xy /= projectedCoord.w;
	projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
	
	return vec3(projectedCoord.xy, depth);
	
}

vec4 rayCast(vec3 dir, inout vec3 hitCoord, out float dDepth){
	dir *= rayStep;
	
	float depth;
	vec4 projectedCoord;
	
	for(int i = 0; i < maxSteps; i++){
		hitCoord += dir;
		projectedCoord = projection * vec4(hitCoord, 1.0);
		projectedCoord.xy /= projectedCoord.w;
		projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
		depth = (view * texture(Position, projectedCoord.xy)).z;
				
		if(depth > 1000.0)
			continue;

		dDepth = hitCoord.z - depth;
		
		if ((dir.z - dDepth) < 1.2) {
		if (dDepth <= 0.0) {
			vec4 Result;
			Result = vec4(binarySearch(dir, hitCoord, dDepth), 1.0);
			return Result;
			}
		}
	}
	return vec4(projectedCoord.xy, depth, 0.0);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}   

vec3 hash(vec3 a){
	a = fract(a * scale);
	a += dot(a, a.yxz + K);
	return fract((a.xxy + a.yxx) * a.zyx);
}	

void main()
{
    vec4 hdrColor = texture(Color, TexCoords);  
	hdrColor = hdrColor * texture(SSAO, TexCoords).r;  
    vec4 bloomColor = texture(Bloom, TexCoords);
    hdrColor += 0.3 * bloomColor;
	
	vec2 metallicEmissive = texture(Data, TexCoords).rg;
	float metallic = metallicEmissive.r;
	
	vec3 viewNormal = mat3(view) * texture(Normal, TexCoords).xyz;
    vec3 viewPosition = (view * texture(Position, TexCoords)).xyz;
	vec3 albedo = hdrColor.rgb;
	float spec = texture(Data, TexCoords).g;
	
	vec3 F0 = vec3(0.04);
	F0		= mix(F0, albedo, metallic);
	vec3 fresnel = fresnelSchlick(max(dot(normalize(viewNormal), normalize(viewPosition)), 0.0), F0);
	
	vec3 reflected = normalize(reflect(normalize(viewPosition), normalize(viewNormal)));
	vec3 hitPos = viewPosition;
	float dDepth;
	
	vec3 worldPos = texture(Position, TexCoords).xyz;
	vec3 jitt = mix(vec3(0.0), normalize(vec3(hash(worldPos))), spec * 0.3);
	vec4 coords = rayCast(jitt + reflected * max(minRayStep, -viewPosition.z), hitPos, dDepth);
	vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - coords.xy));
	float screenEdge = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);
	float multiplier = pow(metallic, reflectionSpecularFalloffExponent) * screenEdge * -reflected.z;
	int ssrMultiplier = 1;
	if(metallic < 0.01) ssrMultiplier = 0;
	
	vec3 ssr = texture(Color, coords.xy).rgb * clamp(multiplier, 0.0, 0.9) * fresnel * ssrMultiplier;
	
	vec3 result = ssr + hdrColor.rgb;
	result = vec3(1.0) - exp(-result * exposure);  
    result = pow(result, vec3(1.0 / gamma));
	
	FragColor = vec4(result, 1.0);
}

###END_FRAGMENT###