#version 430

#include "includes/layersIndexes.glsl"

in float vertex;
in vec3 normal;

//per frame
#include "../include/frameConstants.glsl"

//per model
uniform mat4 modelMatrix;
uniform sampler2DArray tex;
uniform float sqPerLine;
uniform float patchSize;

out vec3 uv;
out vec3 FragPos;
out vec3 OutNormal;

//terrain uniforms
// uniform int width;
// uniform int height;

// gl_VertexID 

vec3 getNormal(vec3 planear){
	const int s = 1;
	const vec2 size = vec2(2*s,0.0);
	const ivec3 off = ivec3(-s,0,s);

	float s11 = texture(tex, planear).r;
    float s01 = textureOffset(tex, planear, off.xy).r;
    float s21 = textureOffset(tex, planear, off.zy).r;
    float s10 = textureOffset(tex, planear, off.yx).r;
    float s12 = textureOffset(tex, planear, off.yz).r;
    vec3 va = normalize(vec3(size.xy,s21-s01));
    vec3 vb = normalize(vec3(size.yx,s12-s10));
    vec4 bump = vec4( cross(va,vb), s11 );

    return vec3(bump.x, bump.z, bump.y);
}

//=================================================================================
void main()
{
	vec3 planear;
	float sqSize = 1 / float(sqPerLine);


	int verticesPerLine = int(sqPerLine) * 6;
	int line = gl_VertexID / verticesPerLine;
	int vertInLine = gl_VertexID % verticesPerLine;
	int sqInLine = vertInLine / 6;
	int vertInSq = vertInLine % 6;

	switch(vertInSq){
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
	planear/=float(sqPerLine);
	planear.x += sqSize*sqInLine;
	planear.y += sqSize*line;

	planear.z = heightmapLayer;

	vec4 height = texture(tex, planear);
	OutNormal = getNormal(planear);
	uv = planear;
	planear*=patchSize;

	vec4 vertexPosition = vec4(planear.x, height.x, planear.y, 1);

    gl_Position = frame.viewProjectionMatrix * modelMatrix * vertexPosition;

    FragPos = vec3(modelMatrix * vertexPosition);
}