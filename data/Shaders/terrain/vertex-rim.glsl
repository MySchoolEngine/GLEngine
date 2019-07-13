#version 430

in float vertex;

//per frame
#include "../include/frameConstants.glsl"

//per model
uniform mat4 modelMatrix;
uniform sampler2D tex;
uniform int sqPerLine;
uniform float patchSize;

out vec4 normal;


//=================================================================================
void main()
{
	vec2 planear;

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

	vec2 texUV = vec2(0,0);

	if(sideIndex==0)
	{
		vertexPosition.x = 0.0f;
		vertexPosition.z = sidePart*patchSize;
		texUV = vec2(0.0, sidePart);
		normal = vec4(1.0,0,0,1.0);
	}
	else if(sideIndex == 1)
	{
		vertexPosition.x = (1.0-sidePart)*patchSize;
		vertexPosition.z = 0.0f;
		texUV = vec2((1.0-sidePart), 0);
		normal = vec4(0,0,-1.0,1.0);
	}
	else if(sideIndex == 3)
	{
		vertexPosition.x = patchSize;
		vertexPosition.z = (1-sidePart)*patchSize;
		texUV = vec2(patchSize, (1-sidePart));
		normal = vec4(0,0,1.0,1.0);
	}
	else
	{
		vertexPosition.x = sidePart*patchSize;
		vertexPosition.z = patchSize;
		texUV = vec2(sidePart, patchSize);
		normal = vec4(1.0,0,0,1.0);
	}


	if(vertexInTriangle == 0)
	{
		vertexPosition.y = texture(tex, texUV).r;
	}
	if(triangleInSq==1 && vertexInTriangle == 1)
	{
		vertexPosition.y = texture(tex, texUV).r;
	}

    gl_Position = frame.projectionMatrix * modelMatrix * vertexPosition;
}