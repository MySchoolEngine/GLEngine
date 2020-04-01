#version 430

#include "includes/layersIndexes.glsl"
#include "../include/frameConstants.glsl"
//per mesh
layout (binding = 0) uniform sampler2DArray tex;
uniform vec3 modelColor[2];
uniform bool hasTexture;
uniform bool selected;
uniform float patchSize;

in vec3 uv;
in vec3 FragPos;

out vec4 fragColor;

vec3 getNormal(){
	const float s = patchSize/textureSize(tex, 0).x;
	const vec2 size = vec2(2*s,0.0);
	const vec3 off = vec3(-s,0,s);

	float s11 = texture(tex, uv).r;
    float s01 = texture(tex, uv + vec3(off.xy, 0)).r;
    float s21 = texture(tex, uv + vec3(off.zy, 0)).r;
    float s10 = texture(tex, uv + vec3(off.yx, 0)).r;
    float s12 = texture(tex, uv + vec3(off.yz, 0)).r;
    vec3 va = normalize(vec3(size.xy,s21-s01));
    vec3 vb = normalize(vec3(size.yx,s12-s10));
    vec4 bump = vec4( cross(va,vb), s11 );

    return vec3(bump.x, bump.z, bump.y);
}

//=================================================================================
float remap(float value,float  low1,float  high1,float  low2,float high2){
	return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
}

//=================================================================================
void main()
{
	vec3 sun = vec3(10,3,-15);
	vec3 normal = normalize(getNormal());
	vec4 upVec = vec4(0,1,0,1);

	vec3 albedo = vec3(1,0,0);
	float cosTheta;

	vec3 terrainTex = texture(tex, uv).rgb;
	float terrainWetness = texture(tex, vec3(uv.xy, wetnessLayer)).r;
	float rockLevel = texture(tex, vec3(uv.xy, Terrain_layer1)).r;

	vec3 mudColor = vec3(0.5, 0.4, 0.278);

	float specularStrength = 0;


	if(hasTexture){
		albedo = terrainTex;
	}
	else{
		albedo = modelColor[1];
		if(rockLevel>=FragPos.y)
		{
			albedo = modelColor[0];
		}
		if(terrainWetness > 30){
			albedo = mix(albedo, mudColor, max(min(remap(terrainWetness, 30,60,0,1),1.0), 0));
			specularStrength = remap(terrainWetness, 20,100,0,0.05);
		}
	}

	// if selected show waterpaths
	if(selected){
		albedo.r = texture(tex, vec3(uv.xy, wetnessLayer)).x;
	}



	cosTheta = dot(normal,normalize(frame.SunPos));
	

	// for faces facing away from sun
	cosTheta = max(0.0, cosTheta);
	//float steepnes = 1.0f - normal.y;

	vec4 MaterialAmbientColor = frame.AmbientStrength * sunColor; // ambient lighting fake
	vec4 MaterialDiffuseColor = cosTheta*sunColor;



//=================================================================================
	vec3 viewDir = normalize(frame.CameraPosition.xyz/frame.CameraPosition.w - FragPos);
	vec3 reflectDir = reflect(-frame.SunPos, normal);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1);
	vec4 MaterialSpecularColor = specularStrength * spec * sunColor;
//=================================================================================


	// vec4 tmp = MaterialDiffuseColor * 2.0;

	//MaterialDiffuseColor += vec4(normal, 1);
	/*if(steepnes < 0.2f){
		fragColor = vec4(1, 0,0,1);
	}
	if((steepnes < 0.7f) && (steepnes >= 0.2f)){
		fragColor = vec4(0, 1,0,1);
	}
	if(steepnes >= 0.7f){
		fragColor = vec4(0, 0,1,1);
	}*/
	fragColor = (MaterialAmbientColor + MaterialDiffuseColor + MaterialSpecularColor) * vec4(albedo, 1);
}