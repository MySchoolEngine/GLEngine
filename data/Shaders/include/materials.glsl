//===================================================
@struct PhongMaterial;
#define MAT_NUMBER 40

layout (std140, binding = 4) uniform materials
{
	PhongMaterial phong[MAT_NUMBER];
};

//===================================================
vec3 getColor(const vec2 uv, const PhongMaterial material)
{
	vec3 color = material.modelColor;
	color *= texture(material.colorMap, uv).xyz;
	return color;
}

//===================================================
vec3 GetNormal(const vec2 uv, const PhongMaterial material, const mat3 TBNMat, const vec3 originalNormal)
{
	if(material.useNormalMap == 0)
	{
    	vec3 norm = normalize(originalNormal);
    	return norm;
	}

	vec3 normalMapSample = texture(material.normalMap, uv).xyz;
    normalMapSample = 2.0 * normalMapSample - vec3(1.0, 1.0, 1.0);

    vec3 normal = TBNMat * normalMapSample;
    normal = normalize(normal);
    //normal.z = -normal.z;
    return normal;
}

//=================================================================================
float GetRoughness(const vec2 uv, const PhongMaterial material)
{
	float roughnessVal = material.roughness;
	roughnessVal *= texture(material.roughnessMap, uv).x;
	return roughnessVal;
}