#version 430

#define dropletsNUM 100

layout (local_size_x = dropletsNUM, local_size_y = 1, local_size_z = 1) in;
layout (binding = 0, rgba32f) uniform image2D terrainPatch;

uniform int numDrops;
uniform int numSteps;
uniform float inertia;

//================================================================================
layout (binding = 1) uniform rainData{
	ivec2[dropletsNUM] droplets;
} rain;

//=================================================================================
struct Droplet
{
  vec2 position;
  vec2 direction;
  float velocity;
  float water;
  float sediment;
};

vec3 GetGradientAndHeight(vec2 posf){
	const ivec3 off = ivec3(-1,0,1);
	const ivec2 pos = ivec2(posf.x, posf.y);
	float x = posf.x - pos.x;
	float y = posf.y - pos.y;
	
	vec4 wave = imageLoad(terrainPatch, pos);
	float s11 = wave.x;
    float NW = imageLoad(terrainPatch, pos+ off.xx).x;//[-1,0]
    float NE = imageLoad(terrainPatch, pos+ off.zx).x;//[1,0]
    float SW = imageLoad(terrainPatch, pos+ off.xz).x;//[0,1]
    float SE = imageLoad(terrainPatch, pos+ off.zz).x;//[1,1]

	float gradX = (NE-NW)*(1-x) + (SE-SW)*x;
	float gradY = (SW-NW)*(1-y) + (SE-NE)*y;

    float height = NW * (1 - x) * (1 - y) + NE * x * (1 - y) + SW * (1 - x) * y + SE * x * y;

    return vec3(gradX, gradY, height);
}

//=================================================================================
void main()
{
	uint posX = gl_GlobalInvocationID.x;
	// simplest way to limit number of droplets I found
	if(posX >= numDrops){
		return;
	}

    Droplet drop;
    ivec2 posI = rain.droplets[posX];
    drop.position = vec2(posI.x, posI.y);
    drop.direction = vec2(0.0, 0.0);
    drop.velocity = 1.0f;// starting speed
    drop.water = 1.0f; // starting water
    drop.sediment = 0.0f;// cause it doesnt took any sediment yet

    // should be parametrisable
    float sedimentCapacityFactor = 1;
    float minSedimentCapacity = 1;

    for(int i = 0;i<numSteps;++i){
		const ivec2 ipos = ivec2(drop.position.x, drop.position.y);
		float offsetInCellx = drop.position.x - ipos.x;
		float offsetInCelly = drop.position.y - ipos.y;
		const vec3 GradientAndGeight = GetGradientAndHeight(drop.position);

		drop.direction.x = (drop.direction.x * inertia - GradientAndGeight.x * (1 - inertia));
        drop.direction.y = (drop.direction.y * inertia - GradientAndGeight.y * (1 - inertia));

        drop.direction = normalize(drop.direction); 

		drop.position += drop.direction;

		if(length(drop.direction)<0.00001)
		{
			break;
		}

        float newHeight = GetGradientAndHeight (drop.position).z;
        float deltaHeight = newHeight - GradientAndGeight.z;

        float sedimentCapacity = max(-deltaHeight * drop.velocity * drop.water * sedimentCapacityFactor, minSedimentCapacity);

        
		float val = imageLoad(terrainPatch, ivec2(drop.position)).x - 4.0f/255.0f;
		imageStore(terrainPatch, ivec2(drop.position), vec4(val, val, val, 1));
	}

}  