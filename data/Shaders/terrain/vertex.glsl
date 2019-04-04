#version 430

in float vertex;
in vec3 normal;

//per frame
#include "../include/frameConstants.glsl"

//per model
uniform mat4 modelMatrix;

//terrain uniforms
uniform int width;
uniform int height;

// gl_VertexID 


//=================================================================================
void main()
{
	int verticesForLine = (width)*2;

	int line = gl_VertexID / verticesForLine;
	int inLineIndex = gl_VertexID % verticesForLine;
	int z = line*2;
	z += (gl_VertexID%2);
	int x = (inLineIndex)/2;

	vec4 vertexPosition = vec4(x, vertex, z, 1);

    gl_Position = frame.projectionMatrix * vertexPosition;
}