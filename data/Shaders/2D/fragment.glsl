#version 430
#if !defined VULKAN
    #extension GL_ARB_bindless_texture : require
#endif

layout(location = 0) in vec2 texCoordOUT;

layout(location = 1) in vec3 ColourOUT;

out vec4 fragColor;
//=================================================================================
void main()
{
	vec2 center = vec2(0.5, 0.5);

	fragColor = vec4(ColourOUT, 1-step(0.5, distance(center, texCoordOUT)));
}