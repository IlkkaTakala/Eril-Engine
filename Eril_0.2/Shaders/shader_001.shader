2;0;
###VERTEX###
#version 430 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_texCoord;
layout (location = 2) in vec3 in_normal;
layout (location = 3) in vec3 in_tangent;

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
	vec3 FragPos;
	mat3 TBN;
} vs_out;

void main()
{
	gl_Position = projection * view * vec4(in_position.x, in_position.y, in_position.z, 1.0);
	vs_out.FragPos = vec3(in_position.x, in_position.y, in_position.z);
	vs_out.TexCoords = in_texCoord;
	
	vec3 T = in_tangent;
	vec3 N = in_normal;
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);

	mat3 TBN = mat3(T, B, N);
	vs_out.TBN = TBN;
}
###END_VERTEX###
###FRAGMENT###
#version 430 core
layout (location = 0) out vec4 PositionBuffer;
layout (location = 1) out vec4 NormalBuffer;
layout (location = 2) out vec4 ColorBuffer;
layout (location = 3) out vec4 DataBuffer;

in VS_OUT {
	vec2 TexCoords;
	vec3 FragPos;
	mat3 TBN;
} fs_in;

uniform sampler2D Albedo;
uniform sampler2D Normal;
uniform sampler2D AO;
uniform sampler2D Roughness;

layout (std140, binding = 0) uniform Globals
{
    mat4 projection;
    mat4 view;
	vec4 viewPos;
	ivec2 screenSize;
	int sceneLightCount;
};

const float height_scale = 0.05;

void main()
{
	//vec2 texCoords = ParallaxMapping(fs_in.TexCoords, normalize(viewPos.xyz - fs_in.FragPos).xyz);
	PositionBuffer = vec4(fs_in.FragPos, 1.0);
	vec3 normal = texture(Normal, fs_in.TexCoords).rgb;
    NormalBuffer = vec4(normalize(fs_in.TBN * normal), 1.0);
    ColorBuffer = texture(Albedo, fs_in.TexCoords);
	DataBuffer = vec4(0.0, 1.0 - texture(Roughness, fs_in.TexCoords).r, texture(AO, fs_in.TexCoords).r, 1.0);
}
###END_FRAGMENT###