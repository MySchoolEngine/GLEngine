#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/UniformBuffer.h>


namespace GLEngine::GLRenderer::Buffers {

//=================================================================================
C_UniformBuffer::C_UniformBuffer(const std::string& blockName, unsigned int index)
	: m_index(static_cast<GLuint>(index))
	, m_blockName(blockName)
	, m_active(false)
{
	C_GLBuffer<GL_UNIFORM_BUFFER>::bind();
	glObjectLabel(GL_BUFFER, m_ID, static_cast<GLsizei>(blockName.size()), blockName.c_str());
	C_GLBuffer<GL_UNIFORM_BUFFER>::unbind();

	glBindBufferBase(GL_UNIFORM_BUFFER, GetIndex(), m_ID);
}

//=================================================================================
void C_UniformBuffer::Activate(bool activate)
{
	if (activate)
	{
		bind();
		m_active = true;
	}
	else
	{
		unbind();
		m_active = false;
	}
}

//=================================================================================
void C_UniformBuffer::AllocateMemory(bool dynamicUsage, const void* initialData /*= nullptr*/)
{
	const auto usage = dynamicUsage ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
	T_Base::AllocateMemory(GetBufferSize(), usage, initialData);
}

} // namespace GLEngine::GLRenderer::Buffers