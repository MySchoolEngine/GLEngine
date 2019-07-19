#version 430

in vec3 vertex;

//per frame
#include "../include/frameConstants.glsl"

//per model

out vec3 uv;
uniform mat4 projMatrix;
uniform mat4 viewMatrix;

//=================================================================================
void main()
{
	uv = vertex;
    gl_Position = frame.viewProjectionMatrix * vec4(vertex, 1.0);
}