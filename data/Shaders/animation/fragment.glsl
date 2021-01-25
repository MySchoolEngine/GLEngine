#version 430

// per mesh
uniform sampler2D tex;
uniform vec4	  modelColor;

in vec3 normalOUT;
in vec2 texCoordOUT;
in vec4 worldCoord;

out vec4 fragColor;

//=================================================================================
void main()
{
	fragColor = texture(tex, texCoordOUT); // vec4(texCoordOUT.xy, 0.0, 0.0);//MaterialDiffuseColor;
}