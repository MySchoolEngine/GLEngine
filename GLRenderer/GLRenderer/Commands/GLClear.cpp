#include <GLRenderer/Commands/GLClear.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdexcept>

namespace GLEngine {
namespace GLRenderer {
namespace Commands {

//=================================================================================
C_GLClear::C_GLClear(E_ClearBits bits)
	: m_Bits(bits)
{

}

//=================================================================================
C_GLClear::C_GLClear(Utils::C_BitField<E_ClearBits> bits)
	: m_Bits(bits)
{

}

//=================================================================================
void C_GLClear::Commit()
{
	glClear(EnumToFlags(m_Bits));
}

//=================================================================================
Renderer::I_RenderCommand::E_Type C_GLClear::GetType() const
{
	return Renderer::I_RenderCommand::E_Type::StateChange;
}

//=================================================================================
std::shared_ptr<Renderer::I_Resource> C_GLClear::GetResource() const
{
	return nullptr;
}

//=================================================================================
int C_GLClear::EnumToFlags(Utils::C_BitField<E_ClearBits>) const
{
	int ret = 0;
	if (m_Bits.CheckFlag(E_ClearBits::Color)) {
		ret |= GL_COLOR_BUFFER_BIT;
	}
	if (m_Bits.CheckFlag(E_ClearBits::Depth)) {
		ret |= GL_DEPTH_BUFFER_BIT;
	}
	if (m_Bits.CheckFlag(E_ClearBits::Stencil)) {
		ret |= GL_STENCIL_BUFFER_BIT;
	}
	if (m_Bits.CheckFlag(E_ClearBits::Accum)) {
		//ret |= GL_ACCUM_BUFFER_BIT;
	}
	return ret;
}

}
}
}