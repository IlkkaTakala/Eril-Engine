2;1;0;
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
	vec3 Normals;
	vec3 BiTangents;
	vec3 Tangents;
} vs_out;

flat out int InstanceID;

uniform mat4 Model;

void main()
{
	InstanceID = gl_InstanceID;
	vec4 pos = in_disp * vec4(in_position, 1.0);
	gl_Position = projection * view * pos;
	vs_out.FragPos = pos;
	vs_out.TexCoords = in_texCoord;
	
	vec3 T = normalize(mat3(Model * in_disp) * in_tangent).xyz;
	vec3 N = normalize(mat3(Model * in_disp) * in_normal).xyz;
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);

	vs_out.Normals = N;
	vs_out.BiTangents = B;
	vs_out.Tangents = T;
}
###END_VERTEX###
###FRAGMENT###
#version 430 core

struct Particle {
	vec4 color;
	vec4 ages;
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

layout(std430, binding = 10) readonly buffer Particles {
	Particle data[];
} ParticlesData;

//layout (location = 0) out vec4 ColorBuffer;
layout (location = 1) out vec4 BloomBuffer;
layout (location = 2) out vec4 accum;
layout (location = 3) out float reveal;

uniform sampler2D Albedo;

in VS_OUT{
	vec2 TexCoords;
	vec4 FragPos;
	vec3 Normals;
	vec3 BiTangents;
	vec3 Tangents;
} fs_in;

flat in int InstanceID;

void main()
{
	vec4 color = texture(Albedo, fs_in.TexCoords) * ParticlesData.data[InstanceID].color;
	//vec4 color = vec4(1.0);

	//const float gamma = 2.2;
	const float exposure = 1.0;
	
	//ColorBuffer = vec4(0.0);
	BloomBuffer = vec4(0.0, 0.0, 0.0, color.a);
	
	float weight = clamp(pow(min(1.0, color.a * 10.0) + 0.01, 3.0) * 1e8 * 
                         pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3);

    // store pixel color accumulation
    accum = vec4(color.rgb * color.a, color.a) * weight;

    // store pixel revealage threshold
    reveal = color.a;

}
###END_FRAGMENT###