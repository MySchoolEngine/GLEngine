#version 430

#define NUM_JOINTS 67
#define MAX_WEIGHTS 3

attribute vec4  vertex;
attribute vec3  normal;
attribute vec2  texCoord;
attribute ivec3 jointIndices; 	// contains ids of joints affecting current vertex
attribute vec3  weights; 		// weights should add to 1 togather

//per frame
#include "../include/frameConstants.glsl"

//per model
uniform mat4 modelMatrix;
uniform mat4 transforms[NUM_JOINTS];
//uniform jointsTran {
//	mat4 transforms[NUM_JOINTS];
//} jointTransforms;

//=================================================================================
out vec3 normalOUT;
out vec2 texCoordOUT;
out vec4 worldCoord;


//=================================================================================
void main()
{

	vec4 totalLocalPos = vec4(0.0);
	vec4 totalNormal = vec4(0.0);
	
	for(int i=0;i<MAX_WEIGHTS;i++){
		mat4 jointTransform = transforms[jointIndices[i]];
		vec4 posePosition = jointTransform * vertex;
		totalLocalPos += posePosition * weights[i];
		
		vec4 worldNormal = jointTransform * vec4(normal, 0.0);
		totalNormal += worldNormal * weights[i];
	}
	normalOUT = totalNormal.xyz;
	texCoordOUT = texCoord;

	worldCoord = modelMatrix * totalLocalPos;

    gl_Position = frame.viewProjectionMatrix * worldCoord;
}