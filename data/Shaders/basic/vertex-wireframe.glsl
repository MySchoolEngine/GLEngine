#version 430

layout (location = 0) in vec4 vertex;

// per frame
#include "../include/frameConstants.glsl"

// per model
uniform mat4 modelMatrix;
uniform vec3 colorIN;

layout (location = 1) out vec3 colorOUT;

void main()
{
	colorOUT	= colorIN;
	gl_Position = frame.viewProjectionMatrix * modelMatrix * vertex;
}