#version 430

layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

uniform float splitRatios[4];
uniform vec2 nearFar;

#include "includes/histogram.glsl"

//=================================================================================
layout(std430, binding = 4) buffer splitFrustums
{
    vec2 frustum[4];
};

//=================================================================================
float remap(float value, float from1, float to1, float from2, float to2)
{
	return (value - from1) / (to1 - from1) * (to2 - from2) + from2;
};

//=================================================================================
float SampleToRealZ(uint index){
	return remap(float(index)/float(HISTOGRAM_SAMPLES), 0.0f, 1.0f, nearFar.x, nearFar.y);
}

//=================================================================================
void main()
{
	frustum[0].x = SampleToRealZ(0);
	uint sum = 0;
	uint actualGroup = 0;
	float groupTrashhold = splitRatios[0] * float(numSamples);
	for(uint i = lowestIndex; i <= highestIndex && actualGroup<4; ++i){
		sum += data_SSBO[i];
		if(sum > groupTrashhold){
			frustum[actualGroup].y = frustum[actualGroup+1].x = SampleToRealZ(i);
			++actualGroup;
			groupTrashhold = float(splitRatios[actualGroup] * numSamples);
		}
	}
	frustum[3].y = SampleToRealZ(highestIndex);
}