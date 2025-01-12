#version 430

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

#include "Plane2D.glsl"
#include "Particle.glsl"

layout(std140, binding = 5) buffer particlesUBO
{
	Particle particles[];
};

uniform float deltaTime;
uniform float particleRadius;
uniform int numParticles;
uniform float DensityRadius;
uniform float PressureMultiplier;

#include "ParticleCalculations.glsl"

vec2 CalculateDensityGradient(in vec2 pos)
{
	const float stepSize = 0.001f;
	const float localDensity = GetLocalDensity(pos);
	const float deltaX = GetLocalDensity(pos + vec2(1,0)*stepSize) - localDensity;
	const float deltaY = GetLocalDensity(pos + vec2(0,1)*stepSize) - localDensity;

	const vec2 gradient = vec2(deltaX, deltaY) / stepSize;
	return gradient;
}

void main(){
	Plane2D planes[3] = Plane2D[3](
		Plane2D(normalize(vec2(0.5, 0.5)), vec2(200, 200)),
		Plane2D(normalize(vec2(-0.5, 0.5)), vec2(600, 200)),
		Plane2D(normalize(vec2(0, -1)), vec2(400, 0))
	);
	const uint particleID = gl_GlobalInvocationID.x;
	if(particleID > numParticles) return;

	Particle particle = particles[particleID];
	particle.LocalDensity = GetLocalDensity(particle.Position);
	particles[particleID] = particle;

	barrier();
	memoryBarrierShared();
	const vec2 previousPos = particle.Position;
	//particle.Velocity += vec2(0, -9.81 * deltaTime);

	const vec2 pressureForce = CalculatePressureForce(particle.Position);
	particle.Velocity += pressureForce * particle.LocalDensity;
	MoveParticle(particle, deltaTime);


	const float dampingFactor = .7f;

	for(int i = 0;i<3; ++i)
	{
		const float Sc = DistanceToPlane(planes[i], previousPos);
		const float Se = DistanceToPlane(planes[i], particle.Position);
		if(Sc*Se <= 0 || Sc <= particleRadius || Se <= particleRadius)
		{
			const float t_freeMove = (Sc - particleRadius) / (Sc - Se);
			particle.Position      = previousPos;
			MoveParticle(particle, deltaTime * t_freeMove);
			particle.Velocity = reflect(particle.Velocity, planes[i].Normal) * dampingFactor;
			MoveParticle(particle, deltaTime * (1.f - t_freeMove));
		}
	}

	particles[particleID] = particle;
}