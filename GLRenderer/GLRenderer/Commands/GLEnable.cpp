#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/GLEnable.h>


namespace GLEngine::GLRenderer::Commands {

//=================================================================================
C_GLEnable::C_GLEnable(E_GLEnableValues value)
	: m_Value(value)
{

}

//=================================================================================
void C_GLEnable::Commit()
{
	glEnable((int)m_Value);
}

//=================================================================================
Renderer::I_RenderCommand::E_Type C_GLEnable::GetType() const
{
	return E_Type::StateChange;
}

//=================================================================================
std::string C_GLEnable::GetDescriptor() const
{
	return std::string("GLEnable");
}

}