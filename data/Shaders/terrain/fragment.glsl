#version 430

//per mesh
uniform vec4 modelColor;

out vec4 fragColor;

//=================================================================================
void main()
{

	fragColor = modelColor;
}