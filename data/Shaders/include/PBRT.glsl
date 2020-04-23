#define PI 3.14159265359

vec3 HalfVector(vec3 omegaIn, vec3 toLight)
{
	return normalize(omegaIn + toLight);
}

//====================================================
// microfacet models
float DistributionGGX(vec3 normal, vec3 halfVector, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(normal, halfVector), 0.001);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

//https://en.wikipedia.org/wiki/Schlick%27s_approximation
float fresnelApprox(float cosTheta, float R0)
{
	return R0 + (1-R0)*pow(1 - cosTheta,5.0);
}



float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.001);
    float NdotL = max(dot(N, L), 0.001);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}