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

flat out int number;

// gl_VertexID 


//=================================================================================
void main()
{
	int sqNumber = gl_VertexID / 6;
	int inRectangle = gl_VertexID%6;

	int line = sqNumber/width;
	int row = sqNumber%width;
	int x = row;
	int z = line;

	if(inRectangle<=2){
		number = sqNumber*2;
	}
	else{
		number = sqNumber*2 + 1;
	}

	switch (inRectangle) {
	case 0:
	break;
	case 1:
		z++;
	break;
	case 2:
		x++;
	break;
	case 3:
		z++;
	break;
	case 4:
		z++;
		x++;
	break;
	case 5:
		x++;
	break;
	}

	vec4 vertexPosition = vec4(x, vertex, z, 1);

    gl_Position = frame.projectionMatrix * vertexPosition;
}