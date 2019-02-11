#version 330

attribute vec4 vertex;

//per frame
#include "../basic/include/frameConstants.glsl"

//per vertex
uniform mat4 modelMatrix;

void main()
{
    gl_Position = frame.projectionMatrix * modelMatrix * vertex;
}