#version 430

#define dropletsNUM 100

layout (local_size_x = dropletsNUM, local_size_y = 1, local_size_z = 1) in;
layout (binding = 0, rgba32f) uniform image2D terrainPatch;

uniform int numDrops;
uniform int numSteps;
uniform float inertia;
uniform float gravityForce;
uniform float evaporate;
uniform float startingSpeed;
uniform float initialWater;
float erodeSpeed;
float depositSpeed;


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

struct BrushData
{
	ivec2 offset;
	float weight;
};

BrushData[5] brush;

//=================================================================================
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


void DepositSediment(vec2 posf, float amountToDeposit)
{
	const ivec3 off = ivec3(-1,0,1);
	const ivec2 pos = ivec2(posf.x, posf.y);
	float offsetInCellx = posf.x - pos.x;
	float offsetInCelly = posf.y - pos.y;

    float NW = imageLoad(terrainPatch, pos+ off.yy).x;//[-1,0]
    float NE = imageLoad(terrainPatch, pos+ off.yz).x;//[1,0]
    float SW = imageLoad(terrainPatch, pos+ off.zy).x;//[0,1]
    float SE = imageLoad(terrainPatch, pos+ off.zz).x;//[1,1]


    NW += amountToDeposit * (1 - offsetInCellx) * (1 - offsetInCelly);
    NE += amountToDeposit * offsetInCellx * (1 - offsetInCelly);
    SW += amountToDeposit * (1 - offsetInCellx) * offsetInCelly;
    SE += amountToDeposit * offsetInCellx * offsetInCelly;

    imageStore(terrainPatch, pos+ off.yy, vec4(NW, NW, NW, 1));
    imageStore(terrainPatch, pos+ off.yz, vec4(NE, NE, NE, 1));
    imageStore(terrainPatch, pos+ off.zy, vec4(SW, SW, SW, 1));
    imageStore(terrainPatch, pos+ off.zz, vec4(SE, SE, SE, 1));
}

//=================================================================================
void main()
{
	brush[0] = BrushData(ivec2(0,0), 0.5);
	brush[1] = BrushData(ivec2(-1,0), 0.0/8.0);
	brush[2] = BrushData(ivec2(1,0), 0.0/8.0);
	brush[3] = BrushData(ivec2(0,-1), 0.0/8.0);
	brush[4] = BrushData(ivec2(0,1), 0.0/8.0);

	uint posX = gl_GlobalInvocationID.x;
	// simplest way to limit number of droplets I found
	if(posX >= numDrops){
		return;
	}

	erodeSpeed = 0.1;
	depositSpeed = 0.1;

    Droplet drop;
    ivec2 posI = rain.droplets[posX];
    drop.position = vec2(posI.x, posI.y);
    drop.direction = vec2(0.0, 0.0);
    drop.velocity = startingSpeed;// starting speed
    drop.water = initialWater; // starting water
    drop.sediment = 0.0f;// cause it doesnt took any sediment yet

    // should be parametrisable
    float sedimentCapacityFactor = 3;
    float minSedimentCapacity = 0;

    for(int i = 0;i<numSteps;++i)
    {
		const ivec2 ipos = ivec2(drop.position.x, drop.position.y);
		float offsetInCellx = drop.position.x - ipos.x;
		float offsetInCelly = drop.position.y - ipos.y;
		const vec3 GradientAndGeight = GetGradientAndHeight(drop.position);
		vec2 positionBak = drop.position;

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

        // flowing uphill
        if(deltaHeight > 0 || drop.sediment > sedimentCapacity)
        {
			float amountToDeposit = (deltaHeight > 0) ? min(deltaHeight, drop.sediment) : (drop.sediment - sedimentCapacity) * depositSpeed;
            drop.sediment -= amountToDeposit;

            DepositSediment(positionBak, amountToDeposit);
    		imageStore(terrainPatch, ivec2(drop.position), vec4(GradientAndGeight.z, GradientAndGeight.z, 255, 1));
    	}
    	else
    	{
			float amountToErode = min ((sedimentCapacity - drop.sediment) * erodeSpeed, -deltaHeight);
			

			for(int i = 0; i< brush.length();++i)
			{
				ivec2 pos = ivec2(drop.position) + brush[i].offset;

				float val = imageLoad(terrainPatch, pos).x;

				float deltaSediment = 0.0f;
				if(val < amountToErode)
				{
					deltaSediment = val;
				}
				else
				{
					deltaSediment = amountToErode;
				}

				float result = val - deltaSediment;
				imageStore(terrainPatch, pos, vec4(result, result, 128, 1));
				drop.sediment += deltaSediment;
			}
    	}

        drop.velocity = sqrt (max(0,drop.velocity * drop.velocity + deltaHeight * gravityForce));
        drop.water *= (1 - evaporate);
        

	}

}  