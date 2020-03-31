#version 430

#include "includes/layersIndexes.glsl"
#include "../include/frameConstants.glsl"

//per mesh
uniform vec3 modelColor[2];
uniform sampler2DArray tex;

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

	cosTheta = dot(normal,normalize(frame.SunPos));
	cosTheta = max(0.0, cosTheta);

	const float layer1Height = texture(tex, vec3(uv, Terrain_layer1)).r;

	if(layer1Height>=FragPos.y)
	{
		albedo = modelColor[0];
	}
	else
	{
		albedo = modelColor[1];
	}

	vec4 MaterialAmbientColor = frame.AmbientStrength * sunColor; // ambient lighting fake
	vec4 MaterialDiffuseColor = cosTheta*sunColor;


	fragColor = (MaterialAmbientColor + MaterialDiffuseColor) * vec4(albedo, 1);
}