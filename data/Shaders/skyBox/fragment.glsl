#version 430

// per mesh
layout(binding = 0) uniform samplerCube tex;

in vec3 uv;


out vec4 fragColor;

//=================================================================================
void main()
{
	fragColor = texture(tex, uv);
}