#version 430

in vec3 vertex;
in vec3 Color;

// per frame
#include "../include/frameConstants.glsl"

out vec3 colorOUT;

void main()
{
	colorOUT	= Color;
	gl_Position = frame.viewProjectionMatrix * vec4(vertex, 1);
}