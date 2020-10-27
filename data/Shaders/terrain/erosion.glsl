#version 430

#define dropletsNUM 100

#include "includes/layersIndexes.glsl"

layout (local_size_x = dropletsNUM, local_size_y = 1, local_size_z = 1) in;
layout (binding = 0, r32f) uniform image2DArray terrainPatch;

uniform int numDrops;
uniform int numSteps;
uniform float inertia;
uniform float gravityForce;
uniform float evaporate;
uniform float startingSpeed;
uniform float initialWater;
uniform float patchSize;
float erodeSpeed;
float depositSpeed;


//================================================================================
layout (binding = 4) uniform rainData{
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

//=================================================================================
struct BrushData
{
	ivec2 offset;
	float weight;
};

//=================================================================================
BrushData[3*3] brush;

//=================================================================================
float gaussian( float x, float mu, float sigma ) {
    float a = ( x - mu ) / sigma;
    return exp( -0.5 * a * a );
}

//=================================================================================
int getTopLayerIndex(ivec2 pos)
{
  for(int layer = Terrain_layer1 + Terrain_NumLayers - 1; layer >= Terrain_layer1; --layer)
  {
    float val = imageLoad(terrainPatch, ivec3(pos, layer)).x;
    if(val > 0)
    {
      return layer;
    }
  }

  return Terrain_layer1;
}

//=================================================================================
float getCurrentHeight(ivec2 pos)
{
  float valAcc = 0;
  for(int layer = Terrain_layer1; layer < Terrain_layer1 + Terrain_NumLayers; ++layer)
  {
    valAcc+=imageLoad(terrainPatch, ivec3(pos, layer)).x;
  }
  return valAcc;
}

//=================================================================================
void GenerateGaussianBrush(int kernelRadius){
  float sigma = kernelRadius/2.;
  float sum = 0;
  int rowColSize = kernelRadius*2+1;

  // compute values
  for (int row = 0; row < rowColSize; row++) {
    for (int col = 0; col < rowColSize; col++) {
      float x = gaussian(row, kernelRadius, sigma)
               * gaussian(col, kernelRadius, sigma);
      brush[row*rowColSize + col] = BrushData(ivec2(row - kernelRadius, col - kernelRadius),x);
      sum += x;
    }
  }
  
  // normalize
  for (int row = 0; row < rowColSize; row++)
    for (int col = 0; col < rowColSize; col++)
      brush[row*rowColSize + col].weight /= sum;

}

//=================================================================================
vec3 GetGradientAndHeight(vec2 posf){
  const ivec3 off = ivec3(-1,0,1);
  const ivec2 pos = ivec2  (posf.x, posf.y);
  float x = posf.x - pos.x;
  float y = posf.y - pos.y;

  float s11 = getCurrentHeight(pos);
  float NW = getCurrentHeight(pos+ off.xx);//[-1,0]
  float NE = getCurrentHeight(pos+ off.zx);//[1,0]
  float SW = getCurrentHeight(pos+ off.xz);//[0,1]
  float SE = getCurrentHeight(pos+ off.zz);//[1,1]

  float gradX = (NE-NW)*(1-x) + (SE-SW)*x;
  float gradY = (SW-NW)*(1-y) + (SE-NE)*y;

  float height = NW * (1 - x) * (1 - y) + NE * x * (1 - y) + SW * (1 - x) * y + SE * x * y;

  return vec3(gradX, gradY, s11);
}

//=================================================================================
void DepositSediment(vec2 posf, float amountToDeposit)
{
	const ivec3 off = ivec3(-1,0,1);
	const ivec2 pos = ivec2(posf.x, posf.y);
	float offsetInCellx = posf.x - pos.x;
	float offsetInCelly = posf.y - pos.y;

	vec4 NWc = imageLoad(terrainPatch, ivec3(pos+ off.yy, Terrain_LayerDirt));
  vec4 NEc = imageLoad(terrainPatch, ivec3(pos+ off.yz, Terrain_LayerDirt));//[1,0]
  vec4 SWc = imageLoad(terrainPatch, ivec3(pos+ off.zy, Terrain_LayerDirt));//[0,1]
  vec4 SEc = imageLoad(terrainPatch, ivec3(pos+ off.zz, Terrain_LayerDirt));//[1,1]

  float NW = NWc.x;//[-1,0]
  float NE = NEc.x;//[1,0]
  float SW = SWc.x;//[0,1]
  float SE = SEc.x;//[1,1]

  float partitionToDeposit = amountToDeposit / 4;

  NW += partitionToDeposit * (1 - offsetInCellx) * (1 - offsetInCelly);
  NE += partitionToDeposit * offsetInCellx * (1 - offsetInCelly);
  SW += partitionToDeposit * (1 - offsetInCellx) * offsetInCelly;
  SE += partitionToDeposit * offsetInCellx * offsetInCelly;

  imageStore(terrainPatch, ivec3(pos+ off.yy, Terrain_LayerDirt), vec4(NW, NWc.y, NWc.z, 1));
  imageStore(terrainPatch, ivec3(pos+ off.yz, Terrain_LayerDirt), vec4(NE, NEc.y, NEc.z, 1));
  imageStore(terrainPatch, ivec3(pos+ off.zy, Terrain_LayerDirt), vec4(SW, SWc.y, SWc.z, 1));
  imageStore(terrainPatch, ivec3(pos+ off.zz, Terrain_LayerDirt), vec4(SE, SEc.y, SEc.z, 1));
}

float GetWettness(vec2 posf)
{
    return imageLoad(terrainPatch, ivec3(posf, wetnessLayer)).r;
}

void SetWettness(vec2 posf, float amount)
{
    imageStore(terrainPatch, ivec3(posf, wetnessLayer), vec4(amount, 0, 0, 0));
}

void WriteWettness(vec2 posf, float amount)
{
    float val =GetWettness(posf);
    SetWettness(posf, val + amount);
}

//=================================================================================
void main()
{
	ivec3 mapSize = imageSize(terrainPatch);

	GenerateGaussianBrush(1);

	uint posX = gl_GlobalInvocationID.x;
	// simplest way to limit number of droplets I found
	if(posX >= numDrops){
		return;
	}

	erodeSpeed = 0.3;
	depositSpeed = 0.3;

  Droplet drop;
  ivec2 posI = rain.droplets[posX];
  drop.position = vec2(posI.x, posI.y);
  drop.direction = vec2(0.0, 0.0);
  drop.velocity = startingSpeed;// starting speed
  drop.water = initialWater; // starting water
  drop.sediment = 0.0f;// cause it doesnt took any sediment yet

  // should be parametrisable
  float sedimentCapacityFactor = 3;
  float minSedimentCapacity = 0.01;

  for(int i = 0;i< numSteps; ++i)
  {
    const ivec2 ipos = ivec2(drop.position.x, drop.position.y);
  	float offsetInCellx = drop.position.x - ipos.x;
  	float offsetInCelly = drop.position.y - ipos.y;
  	const vec3 GradientAndGeight = GetGradientAndHeight(drop.position);
  	vec2 positionBak = drop.position;

  	drop.direction.x = (drop.direction.x * inertia - GradientAndGeight.x * (1 - inertia));
        drop.direction.y = (drop.direction.y * inertia - GradientAndGeight.y * (1 - inertia));

  	if(length(drop.direction)<0.00001)
  	{
  		break;
  	}

    drop.direction = normalize(drop.direction); 

  	drop.position += drop.direction;
    float moveDistance = length(positionBak - drop.position);

    int border = 15;

  	if(drop.position.x < border || drop.position.y < border || drop.position.x >= mapSize.x - border || drop.position.y >= mapSize.y - border)
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
  	}
  	else
  	{
  		const float amountToErode = min ((sedimentCapacity - drop.sediment) * erodeSpeed, -deltaHeight);
  		
      const int layer = getTopLayerIndex(ivec2(positionBak));
      if(layer == Terrain_LayerDirt)
      {
        for(int i = 0; i< brush.length();++i)
        {
          float deltaToErode = amountToErode*brush[i].weight;
          ivec3 pos = ivec3(positionBak + brush[i].offset, Terrain_LayerDirt);

          float val = imageLoad(terrainPatch, pos).x; 

          float deltaSediment = deltaToErode;
          float result = val - deltaSediment;
          if(val < deltaToErode)
          {
            deltaSediment = val;
            result = 0.f;
          }

          imageStore(terrainPatch, pos, vec4(result, 0, 0, 0));
          drop.sediment += deltaSediment;
        }
      }
      else if(layer == Terrain_LayerSand)
      {
        for(int i = 0; i< brush.length();++i)
        {
          float deltaToErode = amountToErode*brush[i].weight;
          ivec3 pos = ivec3(positionBak + brush[i].offset, Terrain_LayerSand);

          float val = imageLoad(terrainPatch, pos).x; 

          float deltaSediment = deltaToErode;
          float result = val - deltaSediment;
          if(val < deltaToErode)
          {
            deltaSediment = val;
            result = 0.f;
          }

          imageStore(terrainPatch, pos, vec4(result, 0, 0, 0));
          drop.sediment += deltaSediment;
        }
      }
  		else if(layer == Terrain_LayerRock)
      {
        if(GetWettness(positionBak) > 20.f)
        {
          WriteWettness(positionBak, -20f);

          // eroding rock
          ivec3 pos = ivec3(positionBak, Terrain_LayerRock);
          vec4 heightVal = imageLoad(terrainPatch, pos);
          float deltaSediment = amountToErode * 0.5f;

          float result = heightVal.x - deltaSediment;
          //WriteWettness(positionBak, deltaSediment);

          imageStore(terrainPatch, pos, vec4(result, 0, 0, 0));

          pos.z = Terrain_LayerSand;
          imageStore(terrainPatch, pos, vec4(deltaSediment, 0, 0, 0));
        }
      }

      // if current action changed top layer to stone lets clear the wetness
      if(layer != Terrain_LayerRock && getTopLayerIndex(ivec2(positionBak)) == Terrain_LayerRock)
      {
        SetWettness(positionBak, 0.0f);
      }
  	}

  	// statistics debug
    WriteWettness(positionBak, drop.water);

    drop.velocity = sqrt (max(0,drop.velocity * drop.velocity + deltaHeight * gravityForce));
    drop.water *= (1 - evaporate);
	}

  DepositSediment(drop.position, drop.sediment);
}