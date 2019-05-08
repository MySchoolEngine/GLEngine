#version 430

#define dropletsNUM 100

layout (local_size_x = dropletsNUM, local_size_y = 1, local_size_z = 1) in;
layout (binding = 0, rgba32f) uniform image2D terrainPatch;

uniform int numDrops;

//================================================================================
layout (binding = 1) uniform rainData{
	ivec2[dropletsNUM] droplets;
} rain;

//=================================================================================
struct Droplet
{
  ivec2 position;
  vec2 direction;
  float velocity;
  float water;
  float sediment;
};

//=================================================================================
void main()
{
	uint posX = gl_GlobalInvocationID.x;
	// simplest way to limit number of droplets I found
	if(posX >= numDrops){
		return;
	}

    Droplet drop;
    drop.position = rain.droplets[posX];
    drop.direction = vec2(0.0, 0.0);
    drop.velocity = 1.0f;// starting speed
    drop.water = 1.0f; // starting water
    drop.sediment = 0.0f;// cause it doesnt took any sediment yet

	float val = imageLoad(terrainPatch, drop.position).x - 50.0f/255.0f;
	imageStore(terrainPatch, drop.position, vec4(0, val, val, 1));
}  