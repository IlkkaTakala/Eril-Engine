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
	vec4 pos = Model * vec4(in_position, 1.0);
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

void main()
{    
	
	vec3 albedo = texture(Albedo, fs_in.TexCoords).rgb;
	
	vec4 color = vec4(albedo, 1.0);
	
	// Height fog
	//float depth = LinearizeDepth(fs_in.FragPos.z) / 100.0;
	//color += clamp(depth - 0.5, 0.0, 8.0);
	
	//const float gamma = 2.2;
	const float exposure = 1.0;
	
	ColorBuffer = color;
	BloomBuffer = clamp(color - exposure, 0.0, 100.0);
}
###END_FRAGMENT###