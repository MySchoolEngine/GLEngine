#version 430

#define NUM_POINTLIGHT 10

#include "../include/frameConstants.glsl"
#include "../include/tracing.glsl"
#include "../include/PBRT.glsl"

//per mesh
uniform sampler2D tex;
uniform sampler2D roughnessMap;
uniform sampler2D colorMap;
uniform sampler2D normalMap;
uniform vec3 modelColor;
uniform float roughness;

uniform bool useRoughnessMap;
uniform bool useColorMap;
uniform bool useNormalMap;

in vec3 normalOUT;
in vec2 texCoordOUT;
in vec4 worldCoord;
in mat3 TBN;

out vec4 fragColor;

@struct pointLight;

layout (std140) uniform lightsUni
{
	pointLight pLight[NUM_POINTLIGHT];
};

float ambientStrength = 0.1;
float specularStrength = 0.5;
vec3 viewPos;
vec3 FragPos;
vec3 usedColor;



float GetRoughness(const vec2 uv)
{
	float roughnessVal = roughness;
	if(useRoughnessMap)
	{
		roughnessVal = texture(roughnessMap, texCoordOUT).x;
	}
	return roughnessVal;
}

vec3 GetNormal()
{
	if(!useNormalMap)
	{
		return normalize(normalOUT);
	}

	vec3 normalMapSample = texture(normalMap, texCoordOUT).xyz;
    normalMapSample = 2.0 * normalMapSample - vec3(1.0, 1.0, 1.0);

    vec3 normal = TBN * normalMapSample;
    normal = normalize(normal);
    return normal;
}


// should be ok ish for dielectrics
const float R0 = 0.04;

vec3 BRDF(const vec3 norm, const vec3 omegaIn, const vec3 omegaOut, const vec3 lightColor, const float roughness)
{
	//cook-torrance brdf
	const vec3 halfVec = HalfVector(omegaOut, omegaIn);
	const float cosTheta = max(dot(norm, omegaIn),0.0);
	const float NDF = DistributionGGX(norm, halfVec, roughness);
	const float G   = GeometrySmith(norm, omegaIn, omegaOut, roughness);
	const float F   = fresnelApprox(cosTheta, R0);

	vec3 Ks = vec3(F, F, F);
	vec3 Kd = vec3(1.0) - Ks;

	float denominator = 4.0 * max(dot(norm, omegaIn), 0.0) * max(dot(norm, omegaOut), 0.0);


	float spec = NDF * F * G/max(denominator, 0.001);

    float NdotL = max(dot(norm, omegaOut), 0.0); 
	return (Kd*usedColor/PI + spec) * lightColor * NdotL;
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
    viewPos = frame.CameraPosition.xyz/frame.CameraPosition.w;
    FragPos = worldCoord.xyz/worldCoord.w;

	float ambientStrength = 0.1;
	float specularStrength = 0.5;
    vec3 ambient = ambientStrength * pLight[0].color;

    usedColor=modelColor;
    if(useColorMap)
    {
    	usedColor = texture(colorMap, texCoordOUT).xyz;
    }

	const vec3 norm = GetNormal();

	vec3 omegaIn = FragPos - viewPos;
	Ray ray;
	ray.origin = FragPos;
	ray.dir = reflect(omegaIn,norm);
	Disc disc;
	disc.plane.normal = vec4(0,0,1,1);
	disc.plane.center = vec3(0,0,-3);
	disc.radiusSq = 10.0;

	vec3 result = vec3(0,0,0);

	float t = 0;
	float distSq; // distance from the middle of a disc
	if(RayDiscIntersect(ray, disc, t, distSq))
	{	
		vec3 omegaOut = normalize(ray.dir);
		vec3 viewDir = normalize(-omegaIn);

		float roughnessVal = GetRoughness(texCoordOUT);

		result += BRDF(norm, viewDir, omegaOut, pLight[0].color, roughnessVal);
	}

	for(int i = 0; i< NUM_POINTLIGHT;++i)
	{
		result += CalculatePointLight(pLight[i], norm);
	}


	fragColor = vec4(result, 1.0);
}