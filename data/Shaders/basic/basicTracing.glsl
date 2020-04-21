#version 430

#define NUM_POINTLIGHT 10

#include "../include/frameConstants.glsl"
#include "../include/tracing.glsl"
#include "../include/PBRT.glsl"

//per mesh
uniform sampler2D tex;
uniform sampler2D roughnessMap;
uniform sampler2D colorMap;
uniform vec3 modelColor;
uniform float roughness;

uniform bool useRoughnessMap;
uniform bool useColorMap;

in vec3 normalOUT;
in vec2 texCoordOUT;
in vec4 worldCoord;

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

// should be ok ish for dielectrics
const float R0 = 0.04;

vec3 CalculatePointLight(pointLight light, vec3 norm)
{
    vec3 ambient = ambientStrength * light.color;

	vec3 lightDir = normalize(light.position - FragPos);  
	
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * light.color;

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm); 

	float roughnessVal = roughness;
	if(useRoughnessMap)
	{
		roughnessVal = texture(roughnessMap, texCoordOUT).x;
	}


	//cook-torrance brdf
	const vec3 halfVec = HalfVector(lightDir, viewDir);
	const float cosTheta = max(dot(norm, viewDir),0.0);
	const float NDF = DistributionGGX(norm, halfVec, roughnessVal);
	const float G   = GeometrySmith(norm, viewDir, lightDir, roughnessVal);
	const float F   = fresnelApprox(cosTheta, R0);

	vec3 Ks = vec3(F, F, F);
	vec3 Kd = vec3(1.0) - Ks;

	float denominator = 4.0 * max(dot(norm, viewDir), 0.0) * max(dot(norm, lightDir), 0.0);


	float spec = NDF * F * G/max(denominator, 0.001);

    float NdotL = max(dot(norm, lightDir), 0.0); 
	return (Kd*usedColor/PI + spec) * light.color * NdotL;
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

	vec3 norm = normalize(normalOUT);

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
	float distSq;
	if(RayDiscIntersect(ray, disc, t, distSq) && false)
	{	
		vec3 lightDir = normalize(ray.dir);  
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = diff * pLight[0].color/(distSq);

		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);  

		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 512);
		vec3 specular = specularStrength * spec * pLight[0].color * pLight[0].intensity /(distSq);  

		result += (ambient + diffuse + specular) * usedColor;

	}
	else{
		//result += ambientStrength * modelColor;
	}

	for(int i = 0; i< NUM_POINTLIGHT;++i)
	{
		result+= CalculatePointLight(pLight[i], norm);
	}


	fragColor = vec4(result, 1.0);
}