#version 430

#include "../include/frameConstants.glsl"

//per mesh
uniform vec4 modelColor;

in vec3 normal;

out vec4 fragColor;

//=================================================================================
void main()
{
	vec4 sun = vec4(0,50,5,1);
	vec4 upVec = vec4(0,1,0,1);

	vec4 MaterialDiffuseColor = vec4(1,0,0,1);
	float cosTheta;

	cosTheta = dot(normal,normalize(frame.SunPos));
	if(cosTheta <= 0)
	{
		fragColor = vec4(0, 0, 0, 1);
		return;
	}

	MaterialDiffuseColor = modelColor;

	vec4 MaterialAmbientColor = 0.6f * MaterialDiffuseColor;
	MaterialDiffuseColor = MaterialAmbientColor + MaterialDiffuseColor * vec4(1.0f, 1.0f, 0.8f, 1.0f) * cosTheta;
	fragColor = MaterialDiffuseColor;
}