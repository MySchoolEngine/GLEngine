#version 430

#define dropletsNUM 100

layout(local_size_x = dropletsNUM, local_size_y = 1, local_size_z = 1) in;
layout (binding = 0, rgba32f) uniform image2D terrainPatch;

uniform int numDrops;

//================================================================================
uniform rainData{
	ivec2[dropletsNUM] droplets;
} rain;

//=================================================================================
void main()
{
	uint posX = gl_GlobalInvocationID.x;
	if(posX >= numDrops){
		return;
	}

	ivec2 dropletSpawn = rain.droplets[posX];

	float val = imageLoad(terrainPatch, dropletSpawn).x - 50.0f/255.0f;
	imageStore(terrainPatch, dropletSpawn, vec4(0, val, val, 1));
}  