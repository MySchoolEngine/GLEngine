#version 430

layout(local_size_x = 1024, local_size_y = 1, local_size_z = 1) in;
layout (binding = 0, rgba32f) writeonly uniform image2D terrainPatch;

//================================================================================
uniform rainData{
	ivec2[100] droplets;
} rain;

//=================================================================================
void main()
{
	uint posX = gl_GlobalInvocationID.x;
	uint posY = gl_GlobalInvocationID.y;
}  