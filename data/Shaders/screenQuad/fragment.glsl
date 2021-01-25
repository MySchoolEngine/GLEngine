#version 400
out vec4 FragColor;
in vec2	 TexCoords;


uniform sampler2D hdrBuffer;
uniform float	  gamma;
uniform float	  exposure;

void main()
{
	vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;

	// Exposure tone mapping
	vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
	// Gamma correction
	mapped = pow(mapped, vec3(1.0 / gamma));

	FragColor = vec4(mapped, 1.0);


	// vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
	// FragColor = vec4(hdrColor, 1.0);
	// FragColor = vec4(TexCoords, 0, 1.0);
}