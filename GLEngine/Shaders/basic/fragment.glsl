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

//================================================================================
in vec4 PSSM_CameraDependentPos;

//=================================================================================
bool IsInPSSMShadow(){
	int split = getNumberFrustum(abs(PSSM_CameraDependentPos.z));
	//if(split == PSSM_SPLITS) return false; // out of frustum, just save a time
	vec4 shadowCoords = pssm.m_LightViewProjection[split] * worldCoord;
	shadowCoords = vec4(shadowCoords.xyz * 0.5 + 0.5, 1.0f);
	float objectNearestLight = texture(shadowMap, vec3(shadowCoords.xy, split)).r;
	float bias = 0.0005*tan(acos(cosTheta));
	bias = clamp(bias, 0.0,0.005);

	return (shadowCoords.z - bias > objectNearestLight);
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


	if(IsInPSSMShadow()){
		//MaterialDiffuseColor = vec4(1.0, 0.0f, 0.0f, 1.0f);
		vec4 MaterialAmbientColor = 0.1f * MaterialDiffuseColor;
		// in this case we add just a little bit of colour to simulate ambient
		MaterialDiffuseColor = MaterialAmbientColor + 
		0.2 * MaterialDiffuseColor * vec4(1.0f, 1.0f, 0.8f, 1.0f);
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