#pragma once

namespace GLEngine {
namespace GLRenderer {
namespace Buffers {
//=================================================================================
template<GLenum TYPE>
C_GLBuffer<TYPE>::C_GLBuffer()
{
	glGenBuffers(1, &m_id);
}

//=================================================================================
template<GLenum TYPE>
C_GLBuffer<TYPE>::~C_GLBuffer()
{
	glDeleteBuffers(1, &m_id);
}

//=================================================================================
template<GLenum TYPE>
void C_GLBuffer<TYPE>::unbind() const
{
	glBindBuffer(TYPE, 0);
}

//=================================================================================
template<GLenum TYPE>
void C_GLBuffer<TYPE>::bind() const
{
	glBindBuffer(TYPE, m_id);
}

//=================================================================================
template<GLenum TYPE>
constexpr GLenum C_GLBuffer<TYPE>::GetBufferType() const noexcept
{
	return TYPE;
}
}}}