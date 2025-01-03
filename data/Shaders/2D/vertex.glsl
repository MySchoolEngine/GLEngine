#version 430

#define uniforms

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 texCoord;
//layout(location = 2) in vec2 Size;
// layout(location = 2) in vec2 Position;
// layout(location = 3) in vec2 Velocity;

struct Particle
{
	vec2 Position;
	vec2 Velocity;
};


layout (std140, binding = 5) buffer particlesUBO
{
	Particle particles[100];
};

// per model
layout(location = 0) uniform mat4 ProjectionMatrix;
layout(location = 1) uniform vec2 Size;
// layout(location = 2) uniform vec2 Position;
//=================================================================================
//layout(location = 0) out vec3 normalOUT;
layout(location = 0) out vec2 texCoordOUT;
layout(location = 1) out vec3 ColourOUT;
//layout(location = 2) out vec4 worldCoord;


//=================================================================================
void main()
{
	vec2 Position = particles[gl_InstanceID].Position;
	vec2 Velocity = particles[gl_InstanceID].Velocity;
	mat4 scale = mat4(1.0);
	scale[0][0] = Size.x;
	scale[1][1] = Size.y;
	scale[3][0] = Position.x;
	scale[3][1] = Position.y;
	vec4 worldCoord = scale * vec4(vertex.xz, 0.0, 1.0);
	texCoordOUT = texCoord;
	ColourOUT = vec3(abs(Velocity)/10, 0);
	gl_Position = ProjectionMatrix * worldCoord;
}