#version 430
#extension GL_ARB_bindless_texture : require

out vec4 FragColor;
in vec2	 TexCoords;

#include "../include/units.glsl"
#include "../include/frameConstants.glsl"
#include "../include/tracing.glsl"
#include "../include/pbrt.glsl"
#include "../include/LightsUBO.glsl"
#include "../include/atmosphere.glsl"

uniform sampler2D hdrBuffer;
uniform sampler2D depthBuffer;

vec2 toClipSpace(vec2 uv)
{
	const float x = (2.0f * uv.x) - 1.0f;
	const float y = 1.0f - (2.0f * uv.y);
	return vec2(x, -y);
}
void main()
{
	vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;

	const vec2 centeredUV = toClipSpace(TexCoords);
	const Ray  ray		  = GetCameraRay(centeredUV);

	const Sphere s = GetAtmosphereBoundary();
	vec3		 intersect;
	vec4		 depth = texture(depthBuffer, TexCoords);
	if (Intersect(ray, s, intersect))
	{
		vec4  intersectionPoint = vec4(ray.origin + ray.dir * intersect.z, 1.0);
		vec4  P					= frame.viewProjectionMatrix * intersectionPoint;
		float depthAtSample		= toLinearDepth(depth.x);

		// this means we doesn't hit the geomtery
		if (P.z < depthAtSample || depthAtSample > frame.zFar)
		{
			hdrColor = GetSkyRadiance(ray, intersect.z);
		}
	}
	FragColor = vec4(hdrColor, 1.0);
}