#version 430
#extension GL_ARB_bindless_texture : require

in vec2	 TexCoords;
out vec4 FragColor;

uniform sampler2D hdrBuffer;
uniform float	  gamma;
uniform float	  exposure;

void main()
{
	vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;

	// hdrColor = left;
	// Exposure tone mapping
	vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
	// Gamma correction
	mapped = pow(mapped, vec3(1.0 / gamma));

	FragColor = vec4(mapped, 1.0);
}