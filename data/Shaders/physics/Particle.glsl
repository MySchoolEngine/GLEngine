struct Particle
{
	vec2 Position;
	vec2 Velocity;
	float LocalDensity;
	float gap;
};

void MoveParticle(inout Particle particle, const float t)
{
	particle.Position += particle.Velocity * t;
}