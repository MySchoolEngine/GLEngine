#version 430

#include "includes/layersIndexes.glsl"
#include "../include/frameConstants.glsl"
//per mesh
layout (binding = 0) uniform sampler2DArray tex;
uniform vec4 modelColor;
uniform bool hasTexture;
uniform bool selected;

in vec3 uv;
in vec3 FragPos;
in vec3 OutNormal;

out vec4 fragColor;

vec3 getNormal(){
	const float s = 0.01;
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
	/* int dist = 6;

	const ivec3 off = ivec3(-1,0,1) * (dist/2);
	
    float R = textureOffset(tex, uv, off.yz).r;//[-1,0]
    float L = textureOffset(tex, uv, off.yx).r;//[1,0]
    float B = textureOffset(tex, uv, off.xy).r;//[0,1]
    float T = textureOffset(tex, uv, off.zy).r;//[1,1]

    return (1.0 / float(dist*2))*vec3(float(dist/2)*(R-L),dist*2, float(dist/2)*(B-T)); */
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

	vec4 MaterialDiffuseColor = vec4(1,0,0,1);
	float cosTheta;

	vec4 terrainTex = texture(tex, uv);
	float terrainWetness = texture(tex, vec3(uv.xy, wetnessLayer)).r;
	vec4 mudColor = vec4(0.5, 0.4, 0.278, 1.0);


	if(hasTexture){
		MaterialDiffuseColor = terrainTex;
	}
	else{
		MaterialDiffuseColor = modelColor;
		if(terrainWetness > 5){
			MaterialDiffuseColor = mix(MaterialDiffuseColor, mudColor, max(min(remap(terrainWetness, 20,60,0,1),1.0), 0));
		}
	}

	// if selected show waterpaths
	if(selected){
		MaterialDiffuseColor.r = texture(tex, vec3(uv.xy, wetnessLayer)).x;
	}



	cosTheta = dot(OutNormal,normalize(frame.SunPos));
	if(cosTheta <= 0)
	{
		fragColor = vec4(1, 0, 0, 1);
		return;
	}

	// for faces facing away from sun
	cosTheta = max(0.0, cosTheta);
	//float steepnes = 1.0f - normal.y;

	vec4 MaterialAmbientColor = 0f * MaterialDiffuseColor; // ambient lighting fake
	MaterialDiffuseColor = MaterialAmbientColor + MaterialDiffuseColor * cosTheta;
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
	fragColor = vec4(OutNormal.rgb, MaterialDiffuseColor.a);//MaterialDiffuseColor;// - tmp + (tmp*0.5);//;//vec4(1,0,0, 0);
	fragColor = MaterialDiffuseColor;
}