#version 430

//per mesh
uniform vec4 modelColor;
uniform sampler2D tex;

in vec2 uv;


out vec4 fragColor;

//=================================================================================
void main()
{
	vec4 sun = vec4(0,50,5,1);

	fragColor = texture(tex, uv);
}