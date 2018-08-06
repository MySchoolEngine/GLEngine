#include "GLW/ShaderProgram.h"

#include "GLW/Buffers/UniformBuffer.h"

#include "Debug/Debug.h"

#include <iostream>

namespace GLW {

	//=================================================================================
	C_ShaderProgram::C_ShaderProgram(GLuint program)
		: m_Program(program)
	{}

	//=================================================================================
	C_ShaderProgram::~C_ShaderProgram()
	{
		glDeleteProgram(m_Program);
#ifdef _DEBUG
		// because of random crashes
		std::cout << "Deleting program: " << m_Program << std::endl;
#endif
		DestructorFullCheck();
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
	void C_ShaderProgram::BindUBO(std::shared_ptr<C_UniformBuffer> ubo)
	{
		int uboBlockLocation = FindUniformBlockLocation(ubo->GetBlockName().c_str());
		if (uboBlockLocation > 0) {
			glUniformBlockBinding(m_Program, uboBlockLocation, ubo->GetBinding());
		}
	}

}