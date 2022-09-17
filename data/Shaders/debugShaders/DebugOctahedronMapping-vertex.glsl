#version 430
#extension GL_ARB_bindless_texture : require

in vec4 vertex;
in vec3 normal;
in vec2 texCoord;
in vec3 tangent;
in vec3 bitangent;

// per frame
#include "../include/frameConstants.glsl"

// per model
uniform mat4 modelMatrix;

//=================================================================================
out vec3 modelPosition;

//=================================================================================
void main()
{
	modelPosition = vertex.xyz / vertex.w;

	const vec4 worldCoord = modelMatrix * vertex;

	gl_Position = frame.viewProjectionMatrix * worldCoord; // tohle je reálná hloubka
}