#version 430
layout(local_size_x = 1, local_size_y = 150, local_size_z = 1) in;

layout (binding = 0, rgba32f) writeonly uniform image2D histagramTex;

uniform vec2 nearFar;

#include "includes/histogram.glsl"
#include "../basic/include/splitFrustrum.glsl"

//=================================================================================
float remap(float value, float from1, float to1, float from2, float to2)
{
	return (value - from1) / (to1 - from1) * (to2 - from2) + from2;
};

//=================================================================================
void main()
{
	uint posX = gl_GlobalInvocationID.x;
	uint posY = gl_GlobalInvocationID.y;

	if(isBorder(posX)){
		imageStore(histagramTex, ivec2(posX, posY), vec4(0, 0, 0, 1));
		return;
	}
	
	if(highestValue!=0 && posY < float(gl_WorkGroupSize.y) / float(highestValue)  * float(data_SSBO[posX])){
		imageStore(histagramTex, ivec2(posX, posY), getColorForFrustum(remap(posX, 0.0f, float(HISTOGRAM_SAMPLES), nearFar.x, nearFar.y)));
	}	
	else{
		imageStore(histagramTex, ivec2(posX, posY), vec4(1, 1, 1, 1));
	}
}