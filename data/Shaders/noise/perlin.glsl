#version 430

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
layout (binding = 0, rgba32f) writeonly uniform image2D perlinNoise;

uniform int patchWidth;
#define PI 3.14159265358979323846
uniform int frequency;
uniform ivec2 unicoord;
uniform bool  usePerlin;

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
    ivec2 coord = ivec2(posX, posY);
    float val = 0;
    if(usePerlin){
        val = frequency*pNoise(unicoord + vec2(posX, posY), 50);
    }
    else{
        val = noise(unicoord + vec2(posX, posY), frequency);
    }

	imageStore(perlinNoise, coord, vec4(val, val, val, 1));
}   