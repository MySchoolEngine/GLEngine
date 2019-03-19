#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/UniformBuffer.h>


namespace GLEngine {
namespace GLRenderer {
namespace Buffers {

//=================================================================================
C_UniformBuffer::C_UniformBuffer(const std::string& blockName, unsigned int index)
	: m_blockName(blockName)
	, m_index(static_cast<GLuint>(index))
{

}

//=================================================================================
C_UniformBuffer::~C_UniformBuffer()
{

}

//=================================================================================
void C_UniformBuffer::bind()
{
	C_GLBuffer<GL_UNIFORM_BUFFER>::bind();
	glBindBufferBase(GL_UNIFORM_BUFFER, GetIndex(), m_id);
}

//=================================================================================
void C_UniformBuffer::Activate(bool activate)
{
	if (activate) {
		bind();
		m_active = true;
	}
	else {
		unbind();
		m_active = false;
	}
}

}}}