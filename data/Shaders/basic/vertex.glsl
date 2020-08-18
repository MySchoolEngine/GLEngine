#version 430
#extension GL_ARB_bindless_texture : require

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

//per frame
#include "../include/frameConstants.glsl"

//per model
uniform mat4 modelMatrix;

//=================================================================================
layout(location = 0) out vec3 normalOUT;
layout(location = 1) out vec2 texCoordOUT;
layout(location = 2) out vec4 worldCoord;
layout(location = 3) out mat3 TBN;

//=================================================================================
void main()
{
	normalOUT = mat3(transpose(inverse(modelMatrix))) * normal;
	texCoordOUT = texCoord;

	worldCoord = modelMatrix * vertex;

	TBN = mat3 (tangent, bitangent, normal);

    gl_Position = frame.viewProjectionMatrix * worldCoord; //tohle je reálná hloubka
}