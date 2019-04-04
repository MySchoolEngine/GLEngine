#include <GLRendererStdafx.h>

#include <GLRenderer/VAO/VAO.h>


namespace GLEngine {
namespace GLRenderer {
namespace VAO {

//=================================================================================
I_GLVAOBae::I_GLVAOBae()
{
	glGenVertexArrays(1, &m_id);
}

//=================================================================================
I_GLVAOBae::~I_GLVAOBae()
{
	glDeleteVertexArrays(1, &m_id);
}

//=================================================================================
void I_GLVAOBae::bind() const
{
	glBindVertexArray(m_id);
}

//=================================================================================
void I_GLVAOBae::unbind() const
{
	glBindVertexArray(0);
}

}}}