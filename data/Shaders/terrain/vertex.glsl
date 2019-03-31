#version 430

in float vertex;
in vec3 normal;

//per frame
#include "../include/frameConstants.glsl"

//per model
uniform mat4 modelMatrix;

//terrain uniforms
uniform int width;//num sq
uniform int height;

flat out int number;

// gl_VertexID 


//=================================================================================
void main()
{
	// width + 1 // you need one more line on the end
	// +1 for restart offset
	int verticesForLine = (width+1)*2;//6

	int line = gl_VertexID / verticesForLine;
	int inLineIndex = gl_VertexID % verticesForLine;
	int z = line;
	if(line%2 == 0){
		z += (gl_VertexID%2);
	}
	else{
		z += ((gl_VertexID+1)%2);
	}
	int x = (inLineIndex)/2;

	vec4 vertexPosition = vec4(x, vertex, z, 1);

    gl_Position = /*frame.projectionMatrix */ vertexPosition;
}