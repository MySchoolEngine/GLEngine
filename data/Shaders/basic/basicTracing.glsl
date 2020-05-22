#version 430

#define NUM_POINTLIGHT 10
#define NUM_AREALIGHT 4

#include "../include/frameConstants.glsl"
#include "../include/tracing.glsl"
#include "../include/PBRT.glsl"

//per mesh
uniform sampler2D tex;
uniform sampler2D roughnessMap;
uniform sampler2D colorMap;
uniform sampler2D normalMap;
uniform sampler2D shadowMap[NUM_AREALIGHT];
uniform vec3 modelColor;
uniform float roughness;
bool twoSided = false;

uniform bool useNormalMap;

in vec3 normalOUT;
in vec2 texCoordOUT;
in vec4 worldCoord;
in mat3 TBN;
in vec4 lightSpacePos;

out vec4 fragColor;

@struct pointLight;
@struct areaLight;

layout (std140) uniform lightsUni
{
	pointLight pLight[NUM_POINTLIGHT];
	areaLight  pAreaLight;
};

float ambientStrength = 0.1;
float specularStrength = 0.5;
vec3 viewPos;
vec3 FragPos;
vec3 usedColor;


void ClipQuadToHorizon(inout vec3 L[5], out int n, out int config)
{
    // detect clipping config
    if (L[0].z > 0.0) config += 1;
    if (L[1].z > 0.0) config += 2;
    if (L[2].z > 0.0) config += 4;
    if (L[3].z > 0.0) config += 8;

    // clip
    n = 0;

    if (config == 0)
    {
        // clip all
    }
    else if (config == 1) // V1 clip V2 V3 V4
    {
        n = 3;
        L[1] = -L[1].z * L[0] + L[0].z * L[1];
        L[2] = -L[3].z * L[0] + L[0].z * L[3];
    }
    else if (config == 2) // V2 clip V1 V3 V4
    {
        n = 3;
        L[0] = -L[0].z * L[1] + L[1].z * L[0];
        L[2] = -L[2].z * L[1] + L[1].z * L[2];
    }
    else if (config == 3) // V1 V2 clip V3 V4
    {
        n = 4;
        L[2] = -L[2].z * L[1] + L[1].z * L[2];
        L[3] = -L[3].z * L[0] + L[0].z * L[3];
    }
    else if (config == 4) // V3 clip V1 V2 V4
    {
        n = 3;
        L[0] = -L[3].z * L[2] + L[2].z * L[3];
        L[1] = -L[1].z * L[2] + L[2].z * L[1];
    }
    else if (config == 5) // V1 V3 clip V2 V4) impossible
    {
        n = 0;
    }
    else if (config == 6) // V2 V3 clip V1 V4
    {
        n = 4;
        L[0] = -L[0].z * L[1] + L[1].z * L[0];
        L[3] = -L[3].z * L[2] + L[2].z * L[3];
    }
    else if (config == 7) // V1 V2 V3 clip V4
    {
        n = 5;
        L[4] = -L[3].z * L[0] + L[0].z * L[3];
        L[3] = -L[3].z * L[2] + L[2].z * L[3];
    }
    else if (config == 8) // V4 clip V1 V2 V3
    {
        n = 3;
        L[0] = -L[0].z * L[3] + L[3].z * L[0];
        L[1] = -L[2].z * L[3] + L[3].z * L[2];
        L[2] =  L[3];
    }
    else if (config == 9) // V1 V4 clip V2 V3
    {
        n = 4;
        L[1] = -L[1].z * L[0] + L[0].z * L[1];
        L[2] = -L[2].z * L[3] + L[3].z * L[2];
    }
    else if (config == 10) // V2 V4 clip V1 V3) impossible
    {
        n = 0;
    }
    else if (config == 11) // V1 V2 V4 clip V3
    {
        n = 5;
        L[4] = L[3];
        L[3] = -L[2].z * L[3] + L[3].z * L[2];
        L[2] = -L[2].z * L[1] + L[1].z * L[2];
    }
    else if (config == 12) // V3 V4 clip V1 V2
    {
        n = 4;
        L[1] = -L[1].z * L[2] + L[2].z * L[1];
        L[0] = -L[0].z * L[3] + L[3].z * L[0];
    }
    else if (config == 13) // V1 V3 V4 clip V2
    {
        n = 5;
        L[4] = L[3];
        L[3] = L[2];
        L[2] = -L[1].z * L[2] + L[2].z * L[1];
        L[1] = -L[1].z * L[0] + L[0].z * L[1];
    }
    else if (config == 14) // V2 V3 V4 clip V1
    {
        n = 5;
        L[4] = -L[0].z * L[3] + L[3].z * L[0];
        L[0] = -L[0].z * L[1] + L[1].z * L[0];
    }
    else if (config == 15) // V1 V2 V3 V4
    {
        n = 4;
    }
    
    if (n == 3)
        L[3] = L[0];
    if (n == 4)
        L[4] = L[0];
}


//=================================================================================
float GetRoughness(const vec2 uv)
{
	float roughnessVal = roughness;
	roughnessVal *= texture(roughnessMap, texCoordOUT).x;
	return roughnessVal;
}

//=================================================================================
vec3 GetNormal()
{
	if(!useNormalMap)
	{
    	vec3 norm = normalize(normalOUT);
    	return norm;
	}

	vec3 normalMapSample = texture(normalMap, texCoordOUT).xyz;
    normalMapSample = 2.0 * normalMapSample - vec3(1.0, 1.0, 1.0);

    vec3 normal = TBN * normalMapSample;
    normal = normalize(normal);
    normal.z = -normal.z;
    return normal;
}


//=================================================================================
// should be ok ish for dielectrics
const float R0 = 0.04;

vec3 BRDF(const vec3 norm, const vec3 omegaIn, const vec3 omegaOut, const vec3 lightColor, const float roughness)
{
	//cook-torrance brdf
	const vec3 halfVec = HalfVector(omegaOut, omegaIn);
	const float cosTheta = max(dot(norm, omegaIn),0.001);
	const float NDF = DistributionGGX(norm, halfVec, roughness);
	const float G   = GeometrySmith(norm, omegaIn, omegaOut, roughness);
	const float F   = fresnelApprox(cosTheta, R0);

	const vec3 Ks = vec3(F, F, F);
	vec3 Kd = vec3(1.0) - Ks;

	const float denominator = 4.0 * max(dot(norm, omegaIn), 0.00001) * max(dot(norm, omegaOut), 0.00001);
	vec3 numerator    = NDF * G * Ks;

	vec3 spec = numerator / max(denominator, 0.00001);

    float NdotL = max(dot(norm, omegaOut), 0.00001); 
	return (Kd*usedColor/PI + spec) * lightColor * NdotL;
}

//=================================================================================
bool isInShadow(const vec4 lightSpaceCoord, const sampler2D shadowMap)
{
	vec3 projCoord = lightSpaceCoord.xyz / lightSpaceCoord.w;
    // transform to [0,1] range
    projCoord = projCoord * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projCoord.xy).r; 

    float currentDepth = projCoord.z;

    return currentDepth > closestDepth;
}

//=================================================================================
vec3 CalculatePointLight(pointLight light, vec3 norm)
{
	// TODO: light intensity
	vec3 lightDir = normalize(light.position - FragPos);

	vec3 viewDir = normalize(viewPos - FragPos);

	float roughnessVal = GetRoughness(texCoordOUT);

	return BRDF(norm, viewDir, lightDir, light.color, roughnessVal);
}

//=================================================================================
void InitRectPoints(const Rect lightRect, out vec3 points[4])
{
    vec3 ex = lightRect.DirX*sqrt(lightRect.width);
    vec3 ey = lightRect.DirY*sqrt(lightRect.height);

    points[0] = lightRect.plane.center - ex - ey;
    points[1] = lightRect.plane.center + ex - ey;
    points[2] = lightRect.plane.center + ex + ey;
    points[3] = lightRect.plane.center - ex + ey;
}

//=================================================================================
float IntegrateEdge(vec3 v1, vec3 v2)
{
    float cosTheta = dot(v1, v2);
    float theta = acos(cosTheta);    
    float res = cross(v1, v2).z * ((theta > 0.001) ? theta/sin(theta) : 1.0);

    return res;
}

vec3 LTC(const mat3 RefFrame, mat3 Minv, const vec3 points[4], const vec3 position)
{
    Minv = RefFrame * Minv;
    vec3 L[5];
    L[0] = Minv * (points[0] - position);
    L[1] = Minv * (points[1] - position);
    L[2] = Minv * (points[2] - position);
    L[3] = Minv * (points[3] - position);

    int n;
    int config = 0;
    ClipQuadToHorizon(L, n, config);

    if(n==0)
        return vec3(0.0,0.0,0.0);


    // project onto sphere
    L[0] = normalize(L[0]);
    L[1] = normalize(L[1]);
    L[2] = normalize(L[2]);
    L[3] = normalize(L[3]);
    L[4] = normalize(L[4]);

    // integrate
    float sum = 0.0;


    sum += IntegrateEdge(L[0], L[1]);
    sum += IntegrateEdge(L[1], L[2]);
    sum += IntegrateEdge(L[2], L[3]);
    if (n >= 4)
        sum += IntegrateEdge(L[3], L[4]);
    if (n == 5)
        sum += IntegrateEdge(L[4], L[0]);

    sum = twoSided ? abs(sum) : max(0.0, sum);

    vec3 Lo_i = vec3(sum, sum, sum);

    return Lo_i;
}

//=================================================================================
vec3 CalculatAreaLight(const areaLight light, const vec3 N, const vec3 V, const vec3 position)
{
    Rect rect;
    rect.plane.normal = vec4(pAreaLight.Normal, 1.f);
    rect.plane.center = pAreaLight.Position;
    rect.DirX = pAreaLight.DirX;
    rect.width = pAreaLight.Width;
    rect.DirY = pAreaLight.DirY;
    rect.height = pAreaLight.Height;

    // construct orthonormal basis around N
    vec3 T1, T2;
    T1 = normalize(V - N*dot(V, N));
    T2 = cross(N, T1);

    mat3 Minv = mat3(1.f);

    const mat3 RefFrame = transpose(mat3(T1, T2, N));

    vec3 points[4];
    InitRectPoints(rect, points);

    vec3 diff = LTC(RefFrame, Minv, points, position);
    vec3 Lo_i = pAreaLight.Intensity * diff * pAreaLight.Color;

    Lo_i /= 2.0*PI;

    return Lo_i * usedColor;
}

//=================================================================================
void main()
{
    viewPos = frame.CameraPosition.xyz/frame.CameraPosition.w;
    FragPos = worldCoord.xyz/worldCoord.w;

    usedColor = modelColor;
	usedColor *= texture(colorMap, texCoordOUT).xyz;

	const vec3 norm = GetNormal();

	vec3 omegaIn = FragPos - viewPos;
	Ray ray;
	ray.origin = FragPos;
	ray.dir = reflect(omegaIn,norm);;

	vec3 result = vec3(0,0,0);

	float t = 0;
	float distSq; // distance from the middle of a disc


    vec3 omegaOut = normalize(ray.dir);
    vec3 viewDir = normalize(-omegaIn);
    //float roughnessVal = GetRoughness(texCoordOUT);

    //if(!isInShadow(lightSpacePos, shadowMap[pAreaLight.ShadowMap]))
        result += CalculatAreaLight(pAreaLight, norm, viewDir, FragPos);

	if(false) for(int i = 0; i< NUM_POINTLIGHT;++i)
	{
		result += CalculatePointLight(pLight[i], norm);
	}


	fragColor = vec4(result, 1.0);
}