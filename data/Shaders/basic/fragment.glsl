#version 430
#extension GL_ARB_bindless_texture : require

#define NUM_POINTLIGHT 10
#define NUM_AREALIGHT 4

#include "../include/frameConstants.glsl"

//per mesh
uniform sampler2D tex;
uniform vec3 modelColor;

in vec3 normalOUT;
in vec2 texCoordOUT;
in vec4 worldCoord;

out vec4 fragColor;

@struct pointLight;
@struct areaLight;

layout (std140) uniform lightsUni
{
	pointLight pLight[NUM_POINTLIGHT];
	areaLight  pAreaLight[NUM_AREALIGHT];
    sampler2D  ltc;
    sampler2D  ltcMag;
};

//=================================================================================
void main()
{
	float ambientStrength = 0.1;
	float specularStrength = 0.5;
    vec3 ambient = ambientStrength * pLight[0].color;
    vec3 viewPos = frame.CameraPosition.xyz/frame.CameraPosition.w;
    vec3 FragPos = worldCoord.xyz/worldCoord.w;


	vec3 norm = normalize(normalOUT);
	vec3 lightDir = normalize(pLight[0].position - FragPos);  
	
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * pLight[0].color;

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);  

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
	vec3 specular = specularStrength * spec * pLight[0].color;  

	vec3 result = (ambient + diffuse + specular) * modelColor;
	fragColor = vec4(result, 1.0);
}
