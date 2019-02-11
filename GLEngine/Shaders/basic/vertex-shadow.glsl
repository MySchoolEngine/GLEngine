#version 330

#include "include/frameConstants.glsl"

attribute vec4 vertex;
attribute vec3 normal;

//per vertex
uniform mat4 modelMatrix;

void main()
{
    gl_Position = frame.projectionMatrix * modelMatrix * (vertex);
}