#version 430

#include "../terrain/includes/layersIndexes.glsl"

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
layout (binding = 0, r32f) writeonly uniform image2DArray perlinNoise;

uniform int patchWidth;
#define PI 3.14159265358979323846
uniform int frequency;
uniform ivec2 unicoord;
uniform bool  usePerlin;
uniform float layerWeight[2];

//=================================================================================
float rand(vec2 c){
    return fract(sin(dot(c.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float noise(vec2 p, float freq ){
    float unit = patchWidth/freq;
    vec2 ij = floor(p/unit);
    vec2 xy = mod(p,unit)/unit;
    //xy = 3.*xy*xy-2.*xy*xy*xy;
    xy = .5*(1.-cos(PI*xy));
    float a = rand((ij+vec2(0.,0.)));
    float b = rand((ij+vec2(1.,0.)));
    float c = rand((ij+vec2(0.,1.)));
    float d = rand((ij+vec2(1.,1.)));
    float x1 = mix(a, b, xy.x);
    float x2 = mix(c, d, xy.x);
    return mix(x1, x2, xy.y);
}

float pNoise(vec2 p, int res){
    float persistance = .5;
    float n = 0.;
    float normK = 0.;
    float f = 4.;
    float amp = 1.;
    int iCount = 0;
    for (int i = 0; i<50; i++){
        n+=amp*noise(p, f);
        f*=2.;
        normK+=amp;
        amp*=persistance;
        if (iCount == res) break;
        iCount++;
    }
    float nf = n/normK;
    return nf*nf*nf*nf;
}

//=================================================================================
void main()
{
	uint posX = gl_GlobalInvocationID.x;
	uint posY = gl_GlobalInvocationID.y;
    float heightAcc = 0.0;
    vec2 imageCoord = vec2(posX, posY);

    float layerWeight[2];
    layerWeight[0] = 0.7;
    layerWeight[1] = 0.3;

    ivec2 randomOffset[2];
    randomOffset[0] = ivec2(0,0);
    randomOffset[1] = ivec2(1500,3000);

    if(usePerlin){
        for(int layer = Terrain_layer1; layer < Terrain_layer1 + Terrain_NumLayers; ++layer)
        {
            const ivec3 coord = ivec3(imageCoord, layer);
            float val = frequency*pNoise(randomOffset[layer - Terrain_layer1] + unicoord + imageCoord, 6);
            val*= layerWeight[layer - Terrain_layer1];

            imageStore(perlinNoise, coord, vec4(val, 0, 0, 0));
            heightAcc+=val;
        }
    }
    else{
        // broken now
        float val = noise(unicoord + vec2(posX, posY), frequency);
    }

    ivec3 coord = ivec3(imageCoord, heightmapLayer);
    imageStore(perlinNoise, coord, vec4(heightAcc, 0, 0, 0));
    imageStore(perlinNoise, ivec3(posX, posY, wetnessLayer), vec4(0, 0, 0, 0));
}