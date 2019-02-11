#version 430

#define PSSM_SPLITS 4

#include "include/splitFrustrum.glsl"
#include "include/pssmUniform.glsl"

uniform sampler2D tex;
uniform sampler2DArray shadowMap;

//per mesh
uniform bool hasTexture;
uniform vec4 modelColor;

in vec3 normalOUT;
in vec4 worldCoord;
varying vec2 texCoordOUT;
in vec4 camPosition;


in vec4 gl_FragCoord;;

out vec4 fragColor;

//globals
vec4 MaterialDiffuseColor;
float cosTheta;

vec2 poissonDisk[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);

//================================================================================
in vec4 PSSM_CameraDependentPos;

//=================================================================================
float IsInPSSMShadow(){
	float inShadow = 0.0f;
	int split = getNumberFrustum(abs(PSSM_CameraDependentPos.z));
	float bias = 0.0005*tan(acos(cosTheta));
	bias = clamp(bias, 0.0,0.005);
	vec4 shadowCoords = pssm.m_LightViewProjection[split] * worldCoord;
	shadowCoords = vec4(shadowCoords.xyz * 0.5 + 0.5, 1.0f);
	for(int i = 0; i<4; ++i){
		float objectNearestLight = texture(shadowMap, vec3(shadowCoords.xy + poissonDisk[i]/700.0, split)).r;

		if(shadowCoords.z - bias > objectNearestLight){
			inShadow += 1.0/4;
		}
	}
	return inShadow;
}

//=================================================================================
void main()
{
	if(hasTexture){
		MaterialDiffuseColor = texture(tex, texCoordOUT);
	}
	else{
		MaterialDiffuseColor = modelColor;
	}


	cosTheta = dot(normalOUT,-1.0f * pssm.SunDirection);

	// make sun colour less blue to simulate real sun

	//vec4 MaterialAmbientColor = 0.1f * MaterialDiffuseColor;
	float inShadow = IsInPSSMShadow();

	if(inShadow > 0.0f){
		//MaterialDiffuseColor = vec4(1.0, 0.0f, 0.0f, 1.0f);
		vec4 MaterialAmbientColor = 0.1f * MaterialDiffuseColor;
		// in this case we add just a little bit of colour to simulate ambient
		MaterialDiffuseColor = MaterialAmbientColor + 
		(1.0 - inShadow + 0.2) * MaterialDiffuseColor * vec4(1.0f, 1.0f, 0.8f, 1.0f) * cosTheta;
	}
	else{
		//MaterialDiffuseColor = vec4(0.0, 1.0f, 0.0f, 1.0f);
		vec4 MaterialAmbientColor = 0.1f * MaterialDiffuseColor;
		// if this fragment is on the light we would like to simulate the angle between the surface
		// and the sun
		MaterialDiffuseColor = MaterialAmbientColor + 
		MaterialDiffuseColor * vec4(1.0f, 1.0f, 0.8f, 1.0f) * cosTheta;
	}

	fragColor = MaterialDiffuseColor;
}