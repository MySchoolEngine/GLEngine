#version 430

#include "../include/frameConstants.glsl"

//per mesh
uniform sampler2D tex;
uniform vec4 modelColor;

in vec3 normalOUT;
in vec2 texCoordOUT;
in vec4 worldCoord;

out vec4 fragColor;

@struct pointLight;

layout (std140) uniform lightsUni
{
	pointLight pLight;
};

//=================================================================================
void main()
{
	vec3 objectColor = vec3(1,0,0);

	float ambientStrength = 0.1;
	float specularStrength = 0.5;
    vec3 ambient = ambientStrength * pLight.color;
    vec3 viewPos = frame.CameraPosition.xyz/frame.CameraPosition.w;
    vec3 FragPos = worldCoord.xyz/worldCoord.w;


	vec3 norm = normalize(normalOUT);
	vec3 lightDir = normalize(pLight.position - FragPos);  
	
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * pLight.color;

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);  

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
	vec3 specular = specularStrength * spec * pLight.color;  

	vec3 result = (ambient + diffuse + specular) * objectColor;
	fragColor = vec4(result, 1.0);
}