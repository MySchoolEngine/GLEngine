#version 430
#if !defined VULKAN
	#extension GL_ARB_bindless_texture : require
#endif ////////

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

// per frame
#include "../include/frameConstants.glsl"

//per model
layout(binding = 2) uniform modelData
{
	mat4 modelMatrix;
	int  materialIndex;
};

//=================================================================================
layout(location = 0) out vec3 normalOUT;
// layout(location = 1) out vec2 texCoordOUT;
// layout(location = 2) out vec4 worldCoord;
// layout(location = 3) out mat3 TBN;

//=================================================================================
void main()
{
	normalOUT	= mat3(transpose(inverse(modelMatrix))) * normal;
	// texCoordOUT = texCoord;

	vec4 worldCoord =  vertex;

	// TBN = mat3(tangent, bitangent, normal);

	gl_Position = frame.viewProjectionMatrix * worldCoord; // tohle je reálná hloubka
}