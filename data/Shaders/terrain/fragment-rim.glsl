#version 430

#include "includes/layersIndexes.glsl"
#include "../include/frameConstants.glsl"

// per mesh
uniform vec3		   modelColor[Terrain_NumLayers];
uniform sampler2DArray tex;

in vec3 normal;
in vec3 FragPos;
in vec2 uv;

out vec4 fragColor;

//=================================================================================
void main()
{
	vec4 sun   = vec4(0, 50, 5, 1);
	vec4 upVec = vec4(0, 1, 0, 1);

	vec3  albedo = vec3(1, 0, 0);
	float cosTheta;

	cosTheta	 = dot(normal, normalize(frame.SunPos));
	cosTheta	 = max(0.0, cosTheta);
	int topLevel = getLayerIndex(tex, uv, FragPos.y);

	albedo = modelColor[topLevel - Terrain_layer1];

	vec4 MaterialAmbientColor = frame.AmbientStrength * sunColor; // ambient lighting fake
	vec4 MaterialDiffuseColor = cosTheta * sunColor;


	fragColor = (MaterialAmbientColor + MaterialDiffuseColor) * vec4(albedo, 1);
}