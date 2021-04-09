#version 430
#extension GL_ARB_bindless_texture : require

#include "includes/layersIndexes.glsl"

in float vertex;
in vec3	 normal;

// per frame
#include "../include/frameConstants.glsl"

// per model
uniform mat4 modelMatrix;
layout(binding = 0) uniform sampler2DArray tex;
uniform float sqPerLine;
uniform float patchSize;

out vec3 uv;
out vec3 FragPos;

// terrain uniforms
// uniform int width;
// uniform int height;

// gl_VertexID

//=================================================================================
void main()
{
	vec3  planear = vec3(0, 0, 0);
	float sqSize  = 1 / float(sqPerLine);


	int verticesPerLine = int(sqPerLine) * 6;
	int line			= gl_VertexID / verticesPerLine;
	int vertInLine		= gl_VertexID % verticesPerLine;
	int sqInLine		= vertInLine / 6;
	int vertInSq		= vertInLine % 6;

	switch (vertInSq)
	{
	case 0:
		planear.x = 0;
		planear.y = 0;
		break;
	case 1:
		planear.x = 0;
		planear.y = 1;
		break;
	case 2:
		planear.x = 1;
		planear.y = 1;
		break;
	case 3:
		planear.x = 1;
		planear.y = 1;
		break;
	case 4:
		planear.x = 1;
		planear.y = 0;
		break;
	case 5:
		planear.x = 0;
		planear.y = 0;
		break;
	}
	planear /= float(sqPerLine);
	planear.x += sqSize * sqInLine;
	planear.y += sqSize * line;

	planear.z = heightmapLayer;

	vec4 height = texture(tex, planear);
	uv			= planear;
	planear *= patchSize;

	vec4 vertexPosition = vec4(planear.x, height.x, planear.y, 1);

	gl_Position = frame.viewProjectionMatrix * modelMatrix * vertexPosition;

	FragPos = vec3(modelMatrix * vertexPosition);
}