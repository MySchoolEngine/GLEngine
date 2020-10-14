//================================================================================
#define NUM_POINTLIGHT 10
#define NUM_AREALIGHT 4

@struct pointLight;
@struct areaLight;
@struct sunLight;

layout (std140) uniform lightsUni
{
	pointLight pLight[NUM_POINTLIGHT];
	areaLight  pAreaLight[NUM_AREALIGHT];
	sunLight   pSunLight;
    sampler2D  ltc;
    sampler2D  ltcMag;
};