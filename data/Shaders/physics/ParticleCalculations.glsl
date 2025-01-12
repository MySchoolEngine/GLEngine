#define PI 3.14159265359

const float mass = 10.f;

float SmoothingKernel(in float radius, in float distance)
{
	const float volume = (PI * pow(radius, 8)) / 4.f; // integral of the kernel
	const float val    = max(0.f, radius*radius - distance*distance);
	return val * val * val / volume;
}

float SmoothingKernelDerivative(in float radius, in float distance)
{
	if(distance >= radius) return 0;
	const float val    = max(0.f, radius*radius - distance*distance);
	float scale = -24 / (PI * pow(radius, 8));
	return -scale * distance * val * val; // minus to keep derivation possitive
}

// Eqn (3)
float GetLocalDensity(in vec2 pos)
{
	float density = 0.f;
	for(int i = 0; i < numParticles; ++i)
	{
		const float distance = distance(particles[i].Position, pos);
		if(distance == 0.0)
			continue;
		density += mass * SmoothingKernel(DensityRadius, distance);
	}
	return density;
}

float ConvertDensityToPressure(in float density)
{
	const float desiredDensity = 1.f;
	const float delta = density - desiredDensity;
	return delta * PressureMultiplier;
}

float GetLocalPropertyValue(in vec2 pos)
{
	float property = 0.f;
	for(int i = 0; i < numParticles; ++i)
	{
		const float distance = distance(particles[i].Position, pos);
		if(distance == 0.0)
			continue;
		property += mass * SmoothingKernel(DensityRadius, distance) / particles[i].LocalDensity;
	}
	return property;
}

vec2 CalculatePressureForce(in vec2 pos)
{
	vec2 pressureForce = vec2(0,0);
	for(int i = 0; i < numParticles; ++i)
	{
		const float distance = distance(particles[i].Position, pos);
		if(distance == 0.0)
			continue;
		const vec2 dir = (particles[i].Position - pos) / distance;
		const float slope = SmoothingKernelDerivative(DensityRadius, distance);
		const float localDensity = particles[i].LocalDensity;
		if(localDensity == 0.f)
		{
			continue;
		}
		pressureForce += ConvertDensityToPressure(localDensity) * dir * slope * mass / localDensity;
	}
	return pressureForce;
}