#version 430
#if !defined VULKAN
#extension GL_ARB_bindless_texture : require
#endif

#include "../include/frameConstants.glsl"
#include "../include/materials.glsl"

//per model
layout(binding = 2) uniform modelData
{
	mat4 modelMatrix;
	int  materialIndex;
};
//===================================================

layout(location = 0) in vec3 normalOUT;
layout(location = 1) in vec2 texCoordOUT;
layout(location = 2) in vec4 worldCoord;
layout(location = 3) in mat3 TBN;

layout(location = 0) out vec4 fragColor;

#include "../include/LightsUBO.glsl"

//=================================================================================
void main()
{
	float ambientStrength  = 0.1;
	float specularStrength = 0.5;
    //vec3 ambient = ambientStrength * pLight[0].color;
    vec3 viewPos = frame.CameraPosition.xyz/frame.CameraPosition.w;
    vec3 FragPos = worldCoord.xyz/worldCoord.w;

	vec3 norm = GetNormal(texCoordOUT, phong[materialIndex], TBN, normalOUT);
	vec3 lightDir = normalize(pLight[0].position - FragPos);  
	

	float diff	  = max(dot(norm, lightDir), 0.0);
	vec3  diffuse = diff * pLight[0].color;

	vec3 viewDir	= normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), phong[materialIndex].Shininess);
	vec3 specular = specularStrength * spec * pLight[0].color;  

	vec3 result = (diffuse + specular) * getColor(texCoordOUT, phong[materialIndex]);
	fragColor = vec4(result, 1.0);
}
