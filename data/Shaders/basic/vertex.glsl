#version 430

attribute vec4 vertex;
attribute vec3 normal;
attribute vec2 texCoord;
attribute vec3 tangent;
attribute vec3 bitangent;

//per frame
#include "../include/frameConstants.glsl"

//per model
uniform mat4 modelMatrix;

//=================================================================================
out vec3 normalOUT;
out vec2 texCoordOUT;
out vec4 worldCoord;
out mat3 TBN;
out vec4 lightSpacePos;



#define NUM_POINTLIGHT 10
@struct pointLight;
@struct areaLight;

layout (std140) uniform lightsUni
{
	pointLight pLight[NUM_POINTLIGHT];
	areaLight  pAreaLight;
};

//=================================================================================
void main()
{
	normalOUT = mat3(transpose(inverse(modelMatrix))) * normal;
	texCoordOUT = texCoord;

	worldCoord = modelMatrix * vertex;

	TBN = mat3 (tangent, bitangent, normal);

    gl_Position = frame.viewProjectionMatrix * worldCoord; //tohle je reálná hloubka
    lightSpacePos = pAreaLight.LightMat * worldCoord;
}