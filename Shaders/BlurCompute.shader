4;3;
###COMPUTE###
#version 430 core

layout (std140, binding = 0) uniform Globals
{
    mat4 projection;
    mat4 view;
	vec4 viewPos;
	ivec2 screenSize;
	int sceneLightCount;
};

layout (binding = 0, rgba32f) uniform image2D First;
layout (binding = 1, rgba32f) uniform image2D Second;
layout (binding = 2, rgba32f) uniform image2D original;

uniform int samples;

float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

#define TILE_SIZE 16
layout(local_size_x = TILE_SIZE, local_size_y = TILE_SIZE, local_size_z = 1) in;
void main()
{
	ivec2 texCoord = ivec2(gl_GlobalInvocationID.xy);
	ivec2 downCoord = texCoord / 2;

	if (gl_GlobalInvocationID % 2 == 0) {
		imageStore(First, downCoord, imageLoad(original, texCoord));
		vec2 tex_offset = 1.0 / vec2(screenSize / 2); // gets size of single texel
		vec3 result = imageLoad(First, downCoord).rgb * weight[0]; // current fragment's contribution
		for(int i = 1; i < 5; ++i)
		{
			result += imageLoad(First, downCoord + ivec2(vec2(tex_offset.x * i, 0.0) * (screenSize.x / 2))).rgb * weight[i];
			result += imageLoad(First, downCoord - ivec2(vec2(tex_offset.x * i, 0.0) * (screenSize.x / 2))).rgb * weight[i];
		}
		
		imageStore(Second, downCoord, vec4(result, 1.0));
		
		result = imageLoad(Second, downCoord).rgb * weight[0]; 

		for(int i = 1; i < 5; ++i)
		{
			result += imageLoad(Second, downCoord + ivec2(vec2(0.0, tex_offset.y * i) * (screenSize.y / 2))).rgb * weight[i];
			result += imageLoad(Second, downCoord - ivec2(vec2(0.0, tex_offset.y * i) * (screenSize.y / 2))).rgb * weight[i];
		}
		imageStore(First, downCoord, vec4(result, 1.0));
	}
	
	barrier();
	
	imageStore(original, texCoord, imageLoad(First, downCoord));
}
###END_COMPUTE###