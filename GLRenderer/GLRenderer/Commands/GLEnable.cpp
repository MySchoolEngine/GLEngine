#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/GLEnable.h>


namespace GLEngine {
namespace GLRenderer {
namespace Commands {

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

}}}