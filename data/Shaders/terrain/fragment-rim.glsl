#version 430
#extension GL_ARB_bindless_texture : require

#include "includes/layersIndexes.glsl"
#include "../include/frameConstants.glsl"
#include "../include/LightsUBO.glsl"

//per mesh
uniform vec3 modelColor[Terrain_NumLayers];
layout (binding = 0) uniform sampler2DArray tex;

in vec3 normal;
in vec3 FragPos;
in vec2 uv;

out vec4 fragColor;

//=================================================================================
void main()
{
	vec4 sun = vec4(0,50,5,1);
	vec4 upVec = vec4(0,1,0,1);

	vec3 albedo = vec3(1,0,0);
	float cosTheta;

	cosTheta = dot(normal,normalize(pSunLight.position));
	cosTheta = max(0.0, cosTheta);
	int topLevel = getLayerIndex(tex, uv, FragPos.y);

	albedo = modelColor[topLevel-Terrain_layer1];

	vec3 MaterialAmbientColor = frame.AmbientStrength * pSunLight.color; // ambient lighting fake
	vec3 MaterialDiffuseColor = cosTheta*pSunLight.color;


	fragColor = vec4((MaterialAmbientColor + MaterialDiffuseColor) * albedo, 1);
}