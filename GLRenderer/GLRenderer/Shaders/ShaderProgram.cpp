#include <GLRendererStdafx.h>

#include <GLRenderer/Shaders/ShaderProgram.h>

//#include "GLW/Buffers/UniformBuffer.h"

//#include "Debug/Debug.h"

#include <glad/glad.h>

#include <iostream>

namespace GLEngine {
namespace GLRenderer {
namespace Shaders {

//=================================================================================
C_ShaderProgram::C_ShaderProgram(GLuint program)
	: m_Program(program)
{
	// TODO: assert(program!=0);
}

//=================================================================================
C_ShaderProgram::~C_ShaderProgram()
{
	glDeleteProgram(m_Program);

	CORE_LOG(E_Level::Info, E_Context::Render, "Deleting program: %d", m_Program);
	//DestructorFullCheck();
}

//=================================================================================
void C_ShaderProgram::useProgram()
{
	glUseProgram(m_Program);
	m_bIsActive = true;
}

//=================================================================================
void C_ShaderProgram::disableProgram()
{
	glUseProgram(0);
	m_bIsActive = false;
}

//=================================================================================
/*void C_ShaderProgram::BindUBO(std::shared_ptr<C_UniformBuffer> ubo)
{
	int uboBlockLocation = FindUniformBlockLocation(ubo->GetBlockName().c_str());
	if (uboBlockLocation > 0) {
		glUniformBlockBinding(m_Program, uboBlockLocation, ubo->GetBinding());
	}
}*/

}
}
}