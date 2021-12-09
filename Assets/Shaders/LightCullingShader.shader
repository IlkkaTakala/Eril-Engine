4;3;
###COMPUTE###
#version 430

struct PointLight {
	vec4 color;
	vec4 positionAndSize;
	vec4 Rotation;
	ivec4 type;
	mat4 transform;
};

struct VisibleIndex {
	int index;
};

layout (std140, binding = 0) uniform Globals
{
    mat4 projection;
    mat4 view;
	vec4 viewPos;
	ivec2 screenSize;
	int sceneLightCount;
};

// Shader storage buffer objects
layout(std430, binding = 1) readonly buffer LightBuffer {
	PointLight data[];
} lightBuffer;

layout(std430, binding = 2) writeonly buffer VisibleLightIndicesBuffer {
	VisibleIndex data[];
} visibleLightIndicesBuffer;

// Uniforms
uniform sampler2D depthMap;

// Shared values between all the threads in the group
shared uint minDepthInt;
shared uint maxDepthInt;
shared uint visibleLightCount;
shared vec4 frustumPlanes[6];
// Shared local storage for visible indices, will be written out to the global buffer at the end
shared int visibleLightIndices[1024];

// Took some light culling guidance from Dice's deferred renderer
// http://www.dice.se/news/directx-11-rendering-battlefield-3/

#define TILE_SIZE 16
layout(local_size_x = TILE_SIZE, local_size_y = TILE_SIZE, local_size_z = 1) in;
void main() {
	ivec2 location = ivec2(gl_GlobalInvocationID.xy);
	ivec2 itemID = ivec2(gl_LocalInvocationID.xy);
	ivec2 tileID = ivec2(gl_WorkGroupID.xy);
	ivec2 tileNumber = ivec2(gl_NumWorkGroups.xy);
	uint index = tileID.y * tileNumber.x + tileID.x;

	// Initialize shared global values for depth and light count
	if (gl_LocalInvocationIndex == 0) {
		minDepthInt = 0xFFFFFFFF;
		maxDepthInt = 0;
		visibleLightCount = 0;
	}

	barrier();

	// Step 1: Calculate the minimum and maximum depth values (from the depth buffer) for this group's tile
	vec2 text = vec2(location) / screenSize;
	float depth = texture(depthMap, text).r;
	// Linearize the depth value from depth buffer (must do this because we created it using projection)
	depth = (0.5 * projection[3][2]) / (depth + 0.5 * projection[2][2] - 0.5);

	// Convert depth to uint so we can do atomic min and max comparisons between the threads
	uint d = floatBitsToUint(depth);

    atomicMin(minDepthInt, d);
    atomicMax(maxDepthInt, d);

	barrier();

	// Step 2: One thread should calculate the frustum planes to be used for this tile
	if (gl_LocalInvocationIndex == 0) {
		// Convert the min and max across the entire tile back to float

		float minDepthZ = uintBitsToFloat(minDepthInt);
		float maxDepthZ = uintBitsToFloat(maxDepthInt);

		vec2 tileScale = vec2(screenSize) * (1.0f / float(2*TILE_SIZE));
		vec2 tileBias = tileScale - vec2(gl_WorkGroupID.xy);

		vec4 c1 = vec4(-projection[0][0] * tileScale.x, 0.0f, tileBias.x, 0.0f);
		vec4 c2 = vec4(0.0f, -projection[1][1] * tileScale.y, tileBias.y, 0.0f);
		vec4 c4 = vec4(0.0f, 0.0f, 1.0f, 0.0f);

		// Derive frustum planes
		// Sides
		//right
		frustumPlanes[0] = c4 - c1;
		//left
		frustumPlanes[1] = c4 + c1;
		//bottom
		frustumPlanes[2] = c4 - c2;
		//top
		frustumPlanes[3] = c4 + c2;
		// Near/far
		frustumPlanes[4] = vec4(0.0f, 0.0f,  1.0f, -minDepthZ);
		frustumPlanes[5] = vec4(0.0f, 0.0f, -1.0f,  maxDepthZ);

		for(int i = 0; i < 4; i++)
		{
			frustumPlanes[i] *= 1.0f / length(frustumPlanes[i].xyz);
		}
	}

	barrier();

	// Step 3: Cull lights.
	// Parallelize the threads against the lights now.
	// Can handle 256 simultaniously. Anymore lights than that and additional passes are performed
	uint threadCount = TILE_SIZE * TILE_SIZE;
    uint passCount = (sceneLightCount + threadCount - 1) /threadCount;
	for (uint passIt = 0; passIt < passCount; passIt++)
	{
		uint lightIndex =  passIt * threadCount + gl_LocalInvocationIndex;

		if (lightIndex >= sceneLightCount) break;
	
		PointLight p = lightBuffer.data[lightIndex];
		vec4 pos = view * vec4(p.positionAndSize.xyz, 1.0);
		float rad = p.positionAndSize.w;

		if (visibleLightCount < 1024)
		{
			bool inFrustum = true;
			if (p.type.x != 0) {
				for (uint i = 3; i > -1 && inFrustum; i--)
				{
					float dist = dot(frustumPlanes[i], pos);
					inFrustum = (-rad <= dist);
				}
			}

			if (inFrustum)
			{
				uint id = atomicAdd(visibleLightCount, 1);
				visibleLightIndices[id] = int(lightIndex);
			}
		}
	}

	barrier();

	if (gl_LocalInvocationIndex == 0) {
		uint offset = index * 1024; // Determine bosition in global buffer
		for (uint i = 0; i < visibleLightCount; i++) {
			visibleLightIndicesBuffer.data[offset + i].index = visibleLightIndices[i];
		}

		if (visibleLightCount != 1024) {
			// Unless we have totally filled the entire array, mark it's end with -1
			// Final shader step will use this to determine where to stop (without having to pass the light count)
			visibleLightIndicesBuffer.data[offset + visibleLightCount].index = -1;
		}
	}
}
###END_COMPUTE###