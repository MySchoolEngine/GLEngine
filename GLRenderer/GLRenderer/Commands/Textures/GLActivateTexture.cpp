#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/Textures/GLActivateTexture.h>


namespace GLEngine {
namespace GLRenderer {
namespace Commands {

//=================================================================================
C_GLActivateTexture::C_GLActivateTexture(unsigned int unit)
	: m_Unit(unit)
{

}

//=================================================================================
void C_GLActivateTexture::Commit()
{
	glActiveTexture(GL_TEXTURE0 + m_Unit);
}

//=================================================================================
Renderer::I_RenderCommand::E_Type C_GLActivateTexture::GetType() const
{
	return E_Type::StateChange;
}

}}}