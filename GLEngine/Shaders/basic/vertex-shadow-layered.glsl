#version 330

attribute vec4 vertex;
attribute vec3 normal;

//per vertex
uniform mat4 modelMatrix;

void main()
{
    gl_Position = modelMatrix * (vertex);
}