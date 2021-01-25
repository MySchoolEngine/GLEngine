#version 430
#extension GL_ARB_bindless_texture : require

#include "../include/frameConstants.glsl"

// per mesh
uniform vec3	  modelColor;
uniform sampler2D colorMap;

in vec3 normalOUT;
in vec2 texCoordOUT;
in vec4 worldCoord;

out vec4 fragColor;

#include "../include/LightsUBO.glsl"


vec3 getColor(vec2 uv)
{
	vec3 color = modelColor;
	color *= texture(colorMap, uv).xyz;
	return color;
}

//=================================================================================
void main()
{
	float ambientStrength  = 0.1;
	float specularStrength = 0.5;
	vec3  ambient		   = ambientStrength * pLight[0].color;
	vec3  viewPos		   = frame.CameraPosition.xyz / frame.CameraPosition.w;
	vec3  FragPos		   = worldCoord.xyz / worldCoord.w;


	vec3 norm	  = normalize(normalOUT);
	vec3 lightDir = normalize(pLight[0].position - FragPos);

	float diff	  = max(dot(norm, lightDir), 0.0);
	vec3  diffuse = diff * pLight[0].color;

	vec3 viewDir	= normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	float spec	   = pow(max(dot(viewDir, reflectDir), 0.0), 64);
	vec3  specular = specularStrength * spec * pLight[0].color;

	vec3 result = (ambient + diffuse + specular) * getColor(texCoordOUT);
	fragColor	= vec4(result, 1.0);
}
