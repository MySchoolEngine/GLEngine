#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/GLViewport.h>

namespace GLEngine::GLRenderer::Commands {

//=================================================================================
C_GLViewport::C_GLViewport(const Renderer::C_Viewport& viewport)
	: m_Viewport(viewport)
{
}

//=================================================================================
void C_GLViewport::Commit()
{
	glViewport(m_Viewport.GetCoordinates().x, m_Viewport.GetCoordinates().y, m_Viewport.GetResolution().x, m_Viewport.GetResolution().y);
}

//=================================================================================
Renderer::I_RenderCommand::E_Type C_GLViewport::GetType() const
{
	return E_Type::StateChange;
}

//=================================================================================
std::string C_GLViewport::GetDescriptor() const
{
	return fmt::format("GLViewport pos: {} size: {}", m_Viewport.GetCoordinates(), m_Viewport.GetResolution());
}

} // namespace GLEngine::GLRenderer::Commands