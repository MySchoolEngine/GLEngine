#version 430
#extension GL_ARB_bindless_texture : require

in float vertex;

//per frame
#include "../include/frameConstants.glsl"
#include "includes/layersIndexes.glsl"

//per model
uniform mat4 modelMatrix;
layout (binding = 0) uniform sampler2DArray tex;
uniform int sqPerLine;
uniform float patchSize;

out vec3 normal;
out vec3 FragPos;
out vec2 uv;


//=================================================================================
void main()
{
	const int verticesPerSide = int(sqPerLine) * 6;

	const int vertexOnSide = gl_VertexID % verticesPerSide;
	const int sideIndex = gl_VertexID / verticesPerSide;
	const int sqOnSide = vertexOnSide / 6;
	const int vertexInSq = vertexOnSide % 6;
	const int triangleInSq = vertexInSq<3 ? 0 : 1;
	const int vertexInTriangle = vertexInSq % 3;

	const float sqSizeUV = 1 / float(sqPerLine);

	float sidePart = sqOnSide * sqSizeUV;
	if(triangleInSq == 0)
	{
		if(vertexInTriangle == 2){
			sidePart += sqSizeUV;
		}
	}
	else
	{
		if(vertexInTriangle != 1)
		{
			sidePart += sqSizeUV;
		}
	}

	vec4 vertexPosition = vec4(0,0.0,0,1);

	vec3 texUV = vec3(0,0,0);

	if(sideIndex==0)
	{
		vertexPosition.x = 0.0f;
		vertexPosition.z = sidePart*patchSize;
		texUV = vec3(0.0, sidePart, 0);
		normal = vec3(-1.0,0,0);
	}
	else if(sideIndex == 1)
	{
		vertexPosition.x = (1.0-sidePart)*patchSize;
		vertexPosition.z = 0.0f;
		texUV = vec3((1.0-sidePart), 0, 0);
		normal = vec3(0,0,-1.0);
	}
	else if(sideIndex == 3)
	{
		vertexPosition.x = patchSize;
		vertexPosition.z = (1-sidePart)*patchSize;
		texUV = vec3(patchSize, (1-sidePart), 0);
		normal = vec3(1.0,0,0);
	}
	else
	{
		vertexPosition.x = sidePart*patchSize;
		vertexPosition.z = patchSize;
		texUV = vec3(sidePart, patchSize, 0);
		normal = vec3(0,0,1);
	}

	texUV.z = heightmapLayer;

	if(vertexInTriangle == 0)
	{
		vertexPosition.y = texture(tex, texUV).r;
	}
	if(triangleInSq==1 && vertexInTriangle == 1)
	{
		vertexPosition.y = texture(tex, texUV).r;
	}

    gl_Position = frame.viewProjectionMatrix * modelMatrix * vertexPosition;
    FragPos = vec3(modelMatrix * vertexPosition);
    uv = texUV.xy;
}