#include <GLRendererStdafx.h>

#include <GLRenderer/VAO/VAO.h>


namespace GLEngine::GLRenderer::VAO {

//=================================================================================
I_GLVAOBase::I_GLVAOBase()
{
	glGenVertexArrays(1, &m_id);
}

//=================================================================================
I_GLVAOBase::~I_GLVAOBase()
{
	glDeleteVertexArrays(1, &m_id);
}

//=================================================================================
void I_GLVAOBase::bind() const
{
	glBindVertexArray(m_id);
}

//=================================================================================
void I_GLVAOBase::unbind() const
{
	glBindVertexArray(0);
}

} // namespace GLEngine::GLRenderer::VAO