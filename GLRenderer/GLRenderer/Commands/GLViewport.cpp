#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/GLViewport.h>

namespace GLEngine::GLRenderer::Commands {

//=================================================================================
C_GLViewport::C_GLViewport(const Renderer::C_Viewport& viewport)
	: m_viewport(viewport)
{
}

//=================================================================================
void C_GLViewport::Commit()
{
	glViewport(m_viewport.GetCoordinates().x, m_viewport.GetCoordinates().y, m_viewport.GetResolution().x, m_viewport.GetResolution().y);
}

//=================================================================================
Renderer::I_RenderCommand::E_Type C_GLViewport::GetType() const
{
	return E_Type::StateChange;
}

//=================================================================================
std::shared_ptr<Renderer::I_Resource> C_GLViewport::GetResource() const
{
	return nullptr;
}

//=================================================================================
std::string C_GLViewport::GetDescriptor() const
{
	return fmt::format("GLViewport pos: {} size: {}", m_viewport.GetCoordinates(), m_viewport.GetResolution());
}

} // namespace GLEngine::GLRenderer::Commands