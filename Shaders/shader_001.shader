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
	// re-orthogonalize T with respect to N
	T = normalize(T - dot(T, N) * N);
	// then retrieve perpendicular vector B with the cross product of T and N
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
uniform sampler2D Displacement;

layout (std140, binding = 0) uniform Globals
{
    mat4 projection;
    mat4 view;
	vec4 viewPos;
	ivec2 screenSize;
	int sceneLightCount;
};

const float height_scale = 0.05;

vec2 ParallaxMapping(vec2 tex, vec3 viewDir)
{
	// number of depth layers
    const float minLayers = 8.0;
	const float maxLayers = 32.0;
	float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDir), 0.0));
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy * height_scale; 
    vec2 deltaTexCoords = P / numLayers;
	
	vec2  currentTexCoords     = tex;
	float currentDepthMapValue = 1.0 - texture(Displacement, currentTexCoords).r;
	  
	while(currentLayerDepth < currentDepthMapValue)
	{
		// shift texture coordinates along direction of P
		currentTexCoords -= deltaTexCoords;
		// get depthmap value at current texture coordinates
		currentDepthMapValue = 1.0 - texture(Displacement, currentTexCoords).r;  
		// get depth of next layer
		currentLayerDepth += layerDepth;  
	}

	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

	// get depth after and before collision for linear interpolation
	float afterDepth  = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = (1.0 - texture(Displacement, prevTexCoords).r) - currentLayerDepth + layerDepth;
	 
	// interpolation of texture coordinates
	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

	return finalTexCoords;
}

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