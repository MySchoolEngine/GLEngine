#version 330

in vec4 vertex;

//per frame
#include "../include/frameConstants.glsl"

//per model
uniform mat4 modelMatrix;
uniform vec3 colorIN;

out vec3 colorOUT;

void main()
{
	colorOUT = colorIN;
    gl_Position = frame.projectionMatrix * modelMatrix * vertex;
}