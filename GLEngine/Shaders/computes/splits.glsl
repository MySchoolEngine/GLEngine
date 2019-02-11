#version 430

layout (binding = 0) uniform sampler2D depthBuffer;


uniform vec2 nearFar;
layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

uniform ivec2 globalSize;

layout(std430, binding = 3) buffer histogram
{
    uint data_SSBO[256];
    uint highestValue; // 
    uint lowestIndex;
    uint highestIndex;
    uint numSamples;
};

//=================================================================================
float remap(float value, float from1, float to1, float from2, float to2)
{
	return (value - from1) / (to1 - from1) * (to2 - from2) + from2;
};

//=================================================================================
float remapRealZ(float value){
    float z_n = 2.0 * value - 1.0;
    return 2.0 * nearFar.x * nearFar.y / (nearFar.y + nearFar.x - z_n * (nearFar.y - nearFar.x));
}


//=================================================================================
void main()
{
	vec2 position = ivec2(gl_GlobalInvocationID.xy);
	if(position.x < globalSize.x && position.y < globalSize.y){
		ivec2       position = ivec2(gl_GlobalInvocationID.xy);
		vec2        screenNormalized = vec2(position) / vec2(globalSize);
		vec4        depthSample = texture(depthBuffer, screenNormalized);
		if(depthSample.r==1.0) return;
		int 		index = int(remap(remapRealZ(depthSample.r), nearFar.x, nearFar.y, 0, 256));
		atomicAdd(data_SSBO[index], 1);
		atomicMax(highestValue, data_SSBO[index]);
		atomicMin(lowestIndex, index);
		atomicMax(highestIndex, index);
		atomicAdd(numSamples, 1);
	}
}