#version 430
#extension GL_ARB_bindless_texture : require

attribute vec4 vertex;
attribute vec3 normal;
attribute vec2 texCoord;
attribute vec3 tangent;
attribute vec3 bitangent;

//per frame
#include "../include/frameConstants.glsl"
#include "waterSurface.glsl"

//per model
uniform mat4 modelMatrix;

//=================================================================================
out vec3 normalOUT;
out vec2 texCoordOUT;
out vec4 worldCoord;
out mat3 TBN;

//=================================================================================
void main()
{
	normalOUT = mat3(transpose(inverse(modelMatrix))) * waterNormal(texCoord, frame.Time);
	texCoordOUT = texCoord;

	worldCoord = modelMatrix * vertex;
	worldCoord.y = waterHeight(texCoord, frame.Time);

	TBN = mat3 (tangent, bitangent, normal);

    gl_Position = frame.viewProjectionMatrix * worldCoord; //tohle je reálná hloubka
}