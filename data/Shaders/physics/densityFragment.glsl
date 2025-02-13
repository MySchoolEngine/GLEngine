#version 430
#extension GL_ARB_bindless_texture : require

#define PI 3.14159265359

out vec4 FragColor;
in vec2	 TexCoords;

// 10px = 1 cm

#include "Particle.glsl"

uniform int numParticles;
uniform float DensityRadius;
uniform uvec2 Dimensions;
uniform float PressureMultiplier;

layout(std140, binding = 5) buffer particlesUBO
{
	Particle particles[100];
};

bool isInEps(float x)
{
	return x > 0 && x < 100.f;
}

#include "ParticleCalculations.glsl"

void main()
{
	const vec2 pos = vec2(TexCoords.x, 1.f-TexCoords.y) * vec2(Dimensions);
	const float density = GetLocalDensity(pos) * 1000;
	
	vec2 pressureForce = CalculatePressureForce(pos);
	if(isInEps(length(pressureForce)))
	{
		FragColor = vec4(1,1,1,0.5);
	}
	else if (length(pressureForce) == 0)
	{
		FragColor = vec4(0,0,0,0);
	}
	else
	{
		FragColor = vec4(pressureForce, 0, 0.5);
	}

	
	const float pressure = ConvertDensityToPressure(density);
	if(isInEps(pressure))
	{
		FragColor = vec4(1,1,1,0.5);
	}
	else if(pressure>0)
	{
		FragColor = vec4(pressure,0,0,0.5);
	}
	else
	{
		FragColor = vec4(0,0,-pressure,0.5);
	}

	/*FragColor = vec4(0,0,0,0);
	{
		vec2 center = vec2(300, 100);// vec2(400, 300);
		const float dst = distance(center, pos);
		if(dst <= DensityRadius)
		{
			float derivative = SmoothingKernelDerivative(DensityRadius, dst) * 100000;
			FragColor = vec4(-derivative, derivative, 0, 0.5);
		}
	}
	{
		vec2 pointForSmoothing = vec2(100, 100);
		const float dst = distance(pointForSmoothing, pos);
		if(dst <= DensityRadius)
		{
			float smoothing = SmoothingKernel(DensityRadius, dst) * 10000;
			FragColor = vec4(-smoothing, smoothing, 0, 0.5);
		}
	}
	*/

}