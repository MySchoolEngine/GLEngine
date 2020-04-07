#define heightmapLayer 0
#define wetnessLayer 1
#define Terrain_layer1 2
#define Terrain_layer2 3 // sand
#define Terrain_layer3 4 // dirt

#define Terrain_LayerRock Terrain_layer1
#define Terrain_LayerSand Terrain_layer2
#define Terrain_LayerDirt Terrain_layer3

#define Terrain_NumLayers 3

const float eps = 0.01f;

int getTopLayerIndex(sampler2DArray terrainTex, vec2 coord)
{
  for(int layer = Terrain_layer1 + Terrain_NumLayers - 1; layer >= Terrain_layer1; --layer)
  {
    float val = texture(terrainTex, vec3(coord.xy, layer)).x;
    if(val > eps)
    {
      return layer;
    }
  }

  return Terrain_layer1 + Terrain_NumLayers-1;
}


int getLayerIndex(sampler2DArray terrainTex, vec2 coord, float height)
{
	float heighAcc = 0.f;
	for(int layer = Terrain_layer1; layer < Terrain_layer1 + Terrain_NumLayers; ++layer)
	{
		heighAcc += texture(terrainTex, vec3(coord.xy, layer)).x;
		if(heighAcc > height)
		{
		  return layer;
		}
	}

  return Terrain_layer1 + Terrain_NumLayers-1;
}