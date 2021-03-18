#pragma once

namespace GLEngine::GLRenderer::Buffers {
//=================================================================================
template <GLenum TYPE> C_GLBuffer<TYPE>::C_GLBuffer()
{
	glGenBuffers(1, &m_id);
}

//=================================================================================
template <GLenum TYPE> C_GLBuffer<TYPE>::~C_GLBuffer()
{
	glDeleteBuffers(1, &m_id);
}

//=================================================================================
template <GLenum TYPE> void C_GLBuffer<TYPE>::unbind() const
{
	glBindBuffer(TYPE, 0);
}

//=================================================================================
template <GLenum TYPE> void C_GLBuffer<TYPE>::bind() const
{
	glBindBuffer(TYPE, m_id);
}

//=================================================================================
template <GLenum TYPE> constexpr GLenum C_GLBuffer<TYPE>::GetBufferType() const noexcept
{
	return TYPE;
}

//=================================================================================
template <GLenum TYPE> void C_GLBuffer<TYPE>::AllocateMemory(const std::size_t size, GLenum usage, const void* data /*= nullptr*/)
{
	// TODO: here should go the memory counting
	bind();
	glBufferData(GetBufferType(), size, data, usage);
	unbind();
}
} // namespace GLEngine::GLRenderer::Buffers