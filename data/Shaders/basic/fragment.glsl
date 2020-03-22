#version 430

//per mesh
uniform sampler2D tex;
uniform vec4 modelColor;

in vec3 normalOUT;
in vec2 texCoordOUT;
in vec4 worldCoord;

out vec4 fragColor;

@struct pointLight;

layout (std140) uniform lightsUni
{
	pointLight pLight;
};

//=================================================================================
void main()
{
	fragColor = vec4(1,0,0,1) / (5* distance(pLight.position, worldCoord.xyz/worldCoord.w)); texture(tex, texCoordOUT);//vec4(texCoordOUT.xy, 0.0, 0.0);//MaterialDiffuseColor;
}