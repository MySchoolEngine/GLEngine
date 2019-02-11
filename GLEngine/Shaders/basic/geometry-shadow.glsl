#version 430

#define PSSM_SPLITS 4

layout(triangles, invocations = PSSM_SPLITS) in;
layout(triangle_strip, max_vertices = 3) out;

#include "include/pssmUniform.glsl"

//================================================================================
void main()
{
	for(int j = 0; j < gl_in.length(); ++j){
		gl_Layer = gl_InvocationID;
		gl_Position = pssm.m_LightViewProjection[gl_InvocationID] * gl_in[j].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}