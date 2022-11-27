#pragma once

#include <glm/glm.hpp>

namespace GLEngine::Renderer {
enum E_BufferType
{
	VertexAttribute, // GL_ARRAY_BUFFER, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
	Uniform,		 // GL_UNIFORM_BUFFER, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT??
};

enum BufferDynamics
{
	Stream,
	Static,
	Dynamic
};

enum BufferAccess
{
	Draw,
	Read,
	Copy
};

struct BufferDescriptor {
	std::string	   name;
	std::size_t	   size;
	E_BufferType   type;
	BufferDynamics dynamics;
	BufferAccess   access;
};

class I_Buffer {
public:
	I_Buffer(const BufferDescriptor& desc)
		: m_Desc(desc)
	{
	}
	const BufferDescriptor& GetDescriptor() const { return m_Desc; }
	virtual void			bind() = 0;

protected:
	BufferDescriptor m_Desc;
};
} // namespace GLEngine::Renderer