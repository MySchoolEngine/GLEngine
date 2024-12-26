#version 430

layout(location = 0) in vec3 vertex;
//layout(location = 1) in vec3 normal;
layout(location = 1) in vec2 texCoord;

// per frame
//#include "../include/frameConstants.glsl"

// per model
uniform mat4 ProjectionMatrix;
uniform vec2 Size;
uniform vec2 Position;

//=================================================================================
//layout(location = 0) out vec3 normalOUT;
layout(location = 0) out vec2 texCoordOUT;
//layout(location = 2) out vec4 worldCoord;


//=================================================================================
void main()
{
	mat4 scale = mat4(1.0);
	scale[0][0] = Size.x;
	scale[1][1] = Size.y;
	scale[3][0] = Position.x;
	scale[3][1] = Position.y;
	vec4 worldCoord = scale * vec4(vertex.xz, 0.0, 1.0);
	texCoordOUT = texCoord;
	gl_Position = ProjectionMatrix * worldCoord;
}