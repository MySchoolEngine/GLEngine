#version 430
#extension GL_ARB_bindless_texture : require

#define NUM_POINTLIGHT 10

#include "../include/frameConstants.glsl"
#include "../include/tracing.glsl"
#include "../include/PBRT.glsl"

// per mesh
uniform sampler2D tex;
uniform sampler2D roughnessMap;
uniform sampler2D colorMap;
uniform sampler2D normalMap;
uniform sampler2D shadowMap;
uniform vec3	  modelColor;
uniform float	  roughness;

uniform bool useNormalMap;

in vec3 normalOUT;
in vec2 texCoordOUT;
in vec4 worldCoord;
in mat3 TBN;

out vec4 fragColor;

#include "../include/LightsUBO.glsl"

float ambientStrength  = 0.1;
float specularStrength = 0.5;
vec3  viewPos;
vec3  FragPos;
vec3  usedColor;


float GetRoughness(const vec2 uv)
{
	float roughnessVal = roughness;
	roughnessVal *= texture(roughnessMap, texCoordOUT).x;
	return roughnessVal;
}

vec3 GetNormal()
{
	if (!useNormalMap)
	{
		vec3 norm = normalize(normalOUT);
		return norm;
	}

	vec3 normalMapSample = texture(normalMap, texCoordOUT).xyz;
	normalMapSample		 = 2.0 * normalMapSample - vec3(1.0, 1.0, 1.0);

	vec3 normal = TBN * normalMapSample;
	normal		= normalize(normal);
	return normal;
}


// should be ok ish for dielectrics
const float R0 = 0.04;

vec3 BRDF(const vec3 norm, const vec3 omegaIn, const vec3 omegaOut, const vec3 lightColor, const float roughness)
{
	// cook-torrance brdf
	const vec3	halfVec	 = HalfVector(omegaOut, omegaIn);
	const float cosTheta = max(dot(norm, omegaIn), 0.0);
	const float NDF		 = DistributionGGX(norm, halfVec, roughness);
	const float G		 = GeometrySmith(norm, omegaIn, omegaOut, roughness);
	const float F		 = fresnelApprox(cosTheta, R0);

	vec3 Ks = vec3(F, F, F);
	vec3 Kd = vec3(1.0) - Ks;

	float denominator = 4.0 * max(dot(norm, omegaIn), 0.0) * max(dot(norm, omegaOut), 0.0);
	vec3  numerator	  = NDF * G * Ks;

	vec3 spec = numerator / max(denominator, 0.001);

	float NdotL = max(dot(norm, omegaOut), 0.0);
	return (Kd * usedColor / PI + spec) * lightColor * NdotL;
}

vec3 CalculatePointLight(pointLight light, vec3 norm)
{
	// TODO: light intensity
	vec3 lightDir = normalize(light.position - FragPos);

	vec3 viewDir = normalize(viewPos - FragPos);

	float roughnessVal = GetRoughness(texCoordOUT);

	return BRDF(norm, viewDir, lightDir, light.color, roughnessVal);
}

//=================================================================================
void main()
{
	usedColor = modelColor;
	usedColor *= texture(colorMap, texCoordOUT).xyz;


	fragColor = vec4(usedColor, 1.0);
}