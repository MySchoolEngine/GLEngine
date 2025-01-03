#version 430

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

#include "Plane2D.glsl"
#include "Particle.glsl"

layout(std430, binding = 5) buffer particlesUBO
{
	Particle particles[];
};

uniform float deltaTime;
uniform int numParticles;

void main(){
	Plane2D planes[2] = Plane2D[2](
		Plane2D(normalize(vec2(0.5, 0.5)), vec2(200, 200)),
		Plane2D(normalize(vec2(-0.5, 0.5)), vec2(600, 200))
	);
	const uint particleID = gl_GlobalInvocationID.x;
	if(particleID > numParticles) return;

	Particle particle = particles[particleID];
	const vec2 previousPos = particle.Position;
	particle.Velocity += vec2(0, -9.81 * deltaTime);
	MoveParticle(particle, deltaTime);

	
	const float dampingFactor = .9f;

	for(int i = 0;i<2; ++i)
	{
		const float Sc = DistanceToPlane(planes[i], previousPos);
		const float Se = DistanceToPlane(planes[i], particle.Position);
		if(Sc*Se <= 0 || Sc <= 15 || Se <= 15)
		{
			const float t_freeMove = (Sc - 15) / (Sc - Se);
			particle.Position      = previousPos;
			MoveParticle(particle, deltaTime * t_freeMove);
			particle.Velocity = reflect(particle.Velocity, planes[i].Normal) * dampingFactor;
			MoveParticle(particle, deltaTime * (1.f - t_freeMove));
		}
	}

	particles[particleID] = particle;
}