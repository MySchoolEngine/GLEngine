#version 430
#extension GL_ARB_bindless_texture : require

#include "../include/units.glsl"
#include "includes/layersIndexes.glsl"
#include "../include/frameConstants.glsl"
#include "../include/LightsUBO.glsl"
#include "../include/PBRT.glsl"
#include "../include/tracing.glsl"
#include "../include/atmosphere.glsl"
// per mesh
layout(binding = 0) uniform sampler2DArray tex;
uniform vec3  modelColor[Terrain_NumLayers];
uniform bool  selected;
uniform float patchSize;

in vec3 uv;
in vec3 FragPos;

out vec4 fragColor;
vec3	 usedColor;

vec3 getNormal()
{
	const float s	 = 3.0 / textureSize(tex, 0).x;
	const vec2	size = vec2(2 * s, 0.0);
	const vec3	off	 = vec3(-s, 0, s);

	float s11  = texture(tex, uv).r;
	float s01  = texture(tex, uv + vec3(off.xy, 0)).r;
	float s21  = texture(tex, uv + vec3(off.zy, 0)).r;
	float s10  = texture(tex, uv + vec3(off.yx, 0)).r;
	float s12  = texture(tex, uv + vec3(off.yz, 0)).r;
	vec3  va   = normalize(vec3(size.xy, s21 - s01));
	vec3  vb   = normalize(vec3(size.yx, s12 - s10));
	vec4  bump = vec4(cross(va, vb), s11);

	return vec3(bump.x, bump.z, bump.y);
}

//=================================================================================
float remap(float value, float low1, float high1, float low2, float high2)
{
	return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
}

vec2 texCoordOUT = vec2(0, 0);

float GetRoughness(vec2 uv)
{
	return 1.0;
}


//=================================================================================
// should be ok ish for dielectrics
const float R0 = 0.04;

vec3 BRDF(const vec3 norm, const vec3 omegaIn, const vec3 omegaOut, const vec3 lightColor, const float roughness)
{
	// cook-torrance brdf
	const vec3	halfVec	 = HalfVector(omegaOut, omegaIn);
	const float cosTheta = max(dot(halfVec, omegaIn), 0.001);
	const float NDF		 = DistributionGGX(norm, halfVec, roughness);
	const float G		 = GeometrySmith(norm, omegaIn, omegaOut, roughness);
	const float F		 = fresnelApprox(cosTheta, R0);

	const vec3 Ks = vec3(F, F, F);
	vec3	   Kd = vec3(1.0) - Ks;

	const float denominator = 4.0 * max(dot(norm, omegaIn), 0.00001) * max(dot(norm, omegaOut), 0.00001);
	vec3		numerator	= NDF * G * vec3(1, 1, 1);

	vec3 spec = numerator / max(denominator, 0.00001);

	float NdotL = max(dot(norm, omegaOut), 0.00001);
	return (Kd * usedColor / PI + spec) * lightColor * NdotL;
}

#include "../include/SunUtils.glsl"

//=================================================================================
void main()
{
	const vec3 normal  = normalize(getNormal());
	const vec3 viewPos = frame.CameraPosition.xyz / frame.CameraPosition.w;
	const vec3 omegaIn = FragPos - viewPos;
	/*Ray ray;
	ray.origin = FragPos;
	ray.dir = reflect(omegaIn,normal);*/
	const vec3 viewDir = normalize(-omegaIn);

	float terrainWetness = texture(tex, vec3(uv.xy, wetnessLayer)).r;
	int	  topLevel		 = getTopLayerIndex(tex, uv.xy);

	vec3 mudColor = vec3(0.5, 0.4, 0.278);

	float specularStrength = 0;


	usedColor = modelColor[topLevel - Terrain_layer1];
	/*if(terrainWetness > 30){
		specularStrength = remap(terrainWetness, 20,1000,0,0.05);
		if(topLevel == Terrain_layer3)
		{
			usedColor = mix(usedColor, mudColor, max(min(remap(terrainWetness, 30,60,0,1),1.0), 0));
		}
	}*/

	// if selected show waterpaths
	/*if(selected){
		usedColor.r = texture(tex, vec3(uv.xy, wetnessLayer)).x/ 100;
		specularStrength = 0;
	}*/

	vec3 result = vec3(0, 0, 0);
	result += CalculatSunLight(normal, viewDir, FragPos);
	fragColor = vec4(result, 1);
}