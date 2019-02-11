#version 430

//per model
in vec3 colorOUT;

out vec4 fragColor;


void main()
{
	fragColor = vec4(colorOUT, 1.0f);
}