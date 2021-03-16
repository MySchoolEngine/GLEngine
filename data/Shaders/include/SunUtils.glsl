// require LightsUBO.glsl
// require tracing.glsl
// require PBRT.glsl
// require atmosphere.glsl

vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

float random(vec3 seed, int i){
    vec4 seed4 = vec4(seed,i);
    float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
    return fract(sin(dot_product) * 43758.5453);
}

//=================================================================================
float SunVisibility(const vec3 position, const vec3 N)
{
    vec4 shadowCoords = pSunLight.viewProjection * vec4(position, 1);
    shadowCoords = vec4(shadowCoords.xyz * 0.5 + 0.5, 1.0f);
    // early out
    /*if(shadowCoords.z < -1 || shadowCoords.z > 1
    || shadowCoords.x < -1 || shadowCoords.x > 1
    || shadowCoords.y < -1 || shadowCoords.y > 1)
    {
      return 0.0;
    }*/
    
    float bias = 0.0005*tan(acos(dot(N, normalize(pSunLight.position))));
    bias = clamp(bias, 0.0,0.005);


    float visibility = 0.0;
    for (int i=0;i<poissonDisk.length();i++){
        const int index = int(poissonDisk.length()*random(position.xyy, i))%poissonDisk.length();
        float objectNearestLight = texture(pSunLight.sunShadowMap, shadowCoords.xy + poissonDisk[index]/1024.0).r;
        if (shadowCoords.z - bias < objectNearestLight){
            visibility+=1.0/poissonDisk.length();
      }
    }

    return visibility; //(shadowCoords.z - bias > objectNearestLight);
}

//=================================================================================
vec3 CalculatSunLight(const vec3 N, const vec3 V, const vec3 position)
{
  const Sphere s = GetAtmosphereBoundary();



  const float visibility = SunVisibility(position, N);
  if(visibility == 0)
  {
      return vec3(0,0,0); // it is in the shadow
  }

  float roughnessVal = GetRoughness(texCoordOUT);

  const Ray r = Ray(position, normalize(pSunLight.position));
  vec3 i;
  Intersect(r, s, i);

  vec3 skyRadiance = GetSkyRadiance(r, i.z);
  SolidAngle sunSA = GetSunSolidAngle(N);

  return (visibility * BRDF(N, V, normalize(pSunLight.position), skyRadiance, roughnessVal)) * Transmittance(r, i.z) * sunSA;
}