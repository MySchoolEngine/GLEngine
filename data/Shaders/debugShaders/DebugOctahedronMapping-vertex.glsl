#version 430
#extension GL_ARB_bindless_texture : require

attribute vec4 vertex;
attribute vec3 normal;
attribute vec2 texCoord;
attribute vec3 tangent;
attribute vec3 bitangent;

// per frame
#include "../include/frameConstants.glsl"

// per model
uniform mat4 modelMatrix;

//=================================================================================
out vec3 modelPosition;

//=================================================================================
void main()
{
	modelPosition = vertex.xyz/vertex.w;

	const vec4 worldCoord = modelMatrix * vertex;

	gl_Position = frame.viewProjectionMatrix * worldCoord; // tohle je reálná hloubka
}