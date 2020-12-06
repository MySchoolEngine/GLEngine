#version 430

//per model
layout (location = 1) in vec3 colorOUT;

out vec4 fragColor;


void main()
{
	fragColor = vec4(colorOUT, 1.0f);
}