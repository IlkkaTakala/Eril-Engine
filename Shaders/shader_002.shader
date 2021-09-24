2;0;
###VERTEX###
#version 430 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_texCoord;
layout (location = 2) in vec3 in_normal;
layout (location = 3) in vec3 in_tangent;
layout (location = 4) in mat4 in_disp;

layout (std140, binding = 0) uniform Globals
{
    mat4 projection;
    mat4 view;
	vec4 viewPos;
	ivec2 screenSize;
	int sceneLightCount;
};

out VS_OUT {
	vec2 TexCoords;
	vec4 FragPos;
	mat3 TBN;
} vs_out;

uniform mat4 Model;

void main()
{
	vec4 pos = Model * in_disp * vec4(in_position, 1.0);
	gl_Position = projection * view * pos;
	vs_out.FragPos = pos;
	vs_out.TexCoords = in_texCoord;
	
	vec3 T = normalize(mat3(Model * in_disp) * in_tangent).xyz;
	vec3 N = normalize(mat3(Model * in_disp) * in_normal).xyz;
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);

	mat3 TBN = mat3(T, B, N);
	vs_out.TBN = TBN;
	//vs_out.normal = normalize(mat3(Model * in_disp) * in_normal).xyz;
}
###END_VERTEX###
###FRAGMENT###
#version 430 core

struct LightData {
	vec4 color;
	vec4 positionAndSize;
	vec4 rotation;
	ivec4 type;
	mat4 transform;
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
layout (location = 2) out vec4 accum;
layout (location = 3) out float reveal;

in VS_OUT {
	vec2 TexCoords;
	vec4 FragPos;
	//vec3 normal;
	mat3 TBN;
} fs_in;

uniform sampler2D Albedo;
uniform sampler2D Normal;
uniform sampler2D Roughness;
uniform sampler2D Metallic;

uniform int numberOfTilesX;

const float PI = 3.14159265359;

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}  

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

const vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
); 

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir, vec3 normal)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = 0; //texture(gShadow, projCoords.xy).r; 
    float currentDepth = projCoords.z;
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005); 
	float shadow = 0.0;
	vec2 texelSize = 1.0 / vec2(1024.0); //textureSize(gShadow, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = 0; //texture(gShadow, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;
	if(projCoords.z > 1.0)
        shadow = 0.0;
    return shadow;
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
	//vec3 FragPos = texture(gPosition, TexCoords).xyz;
	//vec4 data = texture(gData, TexCoords);
	
	vec3 albedo = texture(Albedo, fs_in.TexCoords).rgb;
	float metallic = 0.0;//texture(Metallic, fs_in.TexCoords).r;
	float AO = 1.0;
	float roughness = texture(Roughness, fs_in.TexCoords).r;
	vec3 normal = texture(Normal, fs_in.TexCoords).rgb;
	
	float shadow = 0;
	//float SSAO = texture(gSSAO, TexCoords).r;
	
	vec3 ambient = vec3(0.01, 0.02, 0.06) * albedo * AO;// * SSAO;
	
	vec3 Lo = vec3(0.0);

	vec3 N = normalize(fs_in.TBN * normal); 
    vec3 V = normalize(viewPos - fs_in.FragPos).xyz;

	uint offset = index * 1024;
	
	uint i;
	for (i = 0; i < 1024 && visibleLightIndicesBuffer.data[offset + i].index != -1; i++) {
		uint lightIndex = visibleLightIndicesBuffer.data[offset + i].index;
		LightData light = lightBuffer.data[lightIndex];

		vec3 L = vec3(0.0);
		vec3 H = vec3(0.0);
		vec3 radiance = vec3(0.0);

		switch (light.type.x)
		{
			case 0:
			{
				L = normalize(-light.rotation.xyz);
				H = normalize(V + L);

				float distance 	= 1.0;
				float radius 	= 10000000.0;
				float b 		= 1.0 / (radius * radius * 0.01);
				float attenuation = 1.0 / (1.0 + 0.1 * distance + b * distance * distance);//1.0 / (distance * distance);
				radiance = light.color.rgb * attenuation;
				//shadow = ShadowCalculation(light.transform * vec4(fs_in.FragPos, 1.0), L, N);
			} break;
			
			case 1:
			{
				L = normalize(light.positionAndSize.xyz - fs_in.FragPos.xyz);
				H = normalize(V + L);
			  
				float distance 	= length(light.positionAndSize.xyz - fs_in.FragPos.xyz);
				float radius 	= light.positionAndSize.w;
				float b 		= 1.0 / (radius * radius * 0.01);
				float attenuation = 1.0 / (1.0 + 0.1 * distance + b * distance * distance);//1.0 / (distance * distance);
				radiance = light.color.rgb * attenuation;
				
			} break;
		}
		
		vec3 F0 = vec3(0.04); 
		F0      = mix(F0, albedo, metallic);
		vec3 F  = fresnelSchlick(max(dot(H, V), 0.0), F0);
		
		float NDF = DistributionGGX(N, H, roughness);       
		float G   = GeometrySmith(N, V, L, roughness);
		
		vec3 numerator    = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
		vec3 specular     = numerator / max(denominator, 0.001);
		
		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		  
		kD *= 1.0 - metallic;

		float NdotL = max(dot(N, L), 0.0);
		Lo += (kD * albedo / PI + specular) * radiance * NdotL * (1.0 - shadow);
		
	}
	
	vec4 color = vec4(ambient + Lo, 1.0);
	
	// Height fog
	//float depth = LinearizeDepth(fs_in.FragPos.z) / 100.0;
	//color += clamp(depth - 0.5, 0.0, 8.0);
	
	//const float gamma = 2.2;
	const float exposure = 1.0;
	
	ColorBuffer = color;
	BloomBuffer = clamp(color - exposure, 0.0, 100.0);
}
###END_FRAGMENT###