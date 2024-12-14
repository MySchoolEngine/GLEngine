#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/ShaderStorage.h>

namespace GLEngine::GLRenderer::Buffers {

//=================================================================================
C_ShaderStorageBuffer::C_ShaderStorageBuffer(const int bindingPoint)
	: m_BindingPoint(bindingPoint)
{
}

//=================================================================================
void C_ShaderStorageBuffer::bind() const
{
	C_GLBuffer<GL_SHADER_STORAGE_BUFFER>::bind();
	glBindBufferBase(GetBufferType(), m_BindingPoint, m_ID);
}

} // namespace GLEngine::GLRenderer::Buffers