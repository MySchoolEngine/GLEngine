#version 330

#define PSSM_SPLITS 4

attribute vec4 vertex;
attribute vec3 normal;
attribute vec2 texCoord;

//per frame
#include "include/frameConstants.glsl"

//per model
uniform mat4 modelMatrix;


//=================================================================================
out vec4 PSSM_CameraDependentPos;

#include "include/pssmUniform.glsl"

//=================================================================================
out vec3 normalOUT;
out vec2 texCoordOUT;
out vec4 worldCoord;
out vec4 camPosition;


//=================================================================================
void main()
{
	normalOUT = normal;
	camPosition = pssm.PSSM_CameraView * frame.CameraPosition;
	texCoordOUT = texCoord;

	//to light should be counted from world space

	worldCoord = modelMatrix * vertex;

    gl_Position = frame.projectionMatrix * worldCoord; //tohle je reálná hloubka
    PSSM_CameraDependentPos = pssm.PSSM_CameraView * worldCoord; //tohle je na odvozeni hloubek
}