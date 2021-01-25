#version 430

#include "../terrain/includes/layersIndexes.glsl"

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
layout(binding = 0, r32f) uniform image2DArray heightmap;


//=================================================================================
void main()
{
	uint posX		= gl_GlobalInvocationID.x;
	uint posY		= gl_GlobalInvocationID.y;
	vec2 imageCoord = vec2(posX, posY);

	float valAcc = 0;
	for (int layer = Terrain_layer1; layer < Terrain_layer1 + Terrain_NumLayers; ++layer)
	{
		const ivec3 coord = ivec3(imageCoord, layer);
		float		val	  = imageLoad(heightmap, coord).x;
		valAcc += val;
	}

	imageStore(heightmap, ivec3(imageCoord, heightmapLayer), vec4(valAcc, 0, 0, 0));
}