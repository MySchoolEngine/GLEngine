#version 430

attribute vec4 vertex;
attribute vec3 normal;
attribute vec2 texCoord;

//per frame
#include "../include/frameConstants.glsl"

//per model
uniform mat4 modelMatrix;

//=================================================================================
out vec3 normalOUT;
out vec2 texCoordOUT;
out vec4 worldCoord;


//=================================================================================
void main()
{
	normalOUT = mat3(transpose(inverse(modelMatrix))) * normal;
	texCoordOUT = texCoord;

	worldCoord = modelMatrix * vertex;

    gl_Position = frame.viewProjectionMatrix * worldCoord; //tohle je reálná hloubka
}