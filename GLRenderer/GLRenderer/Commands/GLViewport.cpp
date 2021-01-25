#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/GLViewport.h>

namespace GLEngine { namespace GLRenderer { namespace Commands {

//=================================================================================
C_GLViewport::C_GLViewport(int x, int y, int widht, int height)
	: m_coord(x, y)
	, m_Dimensions(widht, height)
{
}

//=================================================================================
C_GLViewport::C_GLViewport(glm::ivec2 coord, int widht, int height)
	: m_coord(coord)
	, m_Dimensions(widht, height)
{
}

//=================================================================================
void C_GLViewport::Commit()
{
	glViewport(m_coord.x, m_coord.y, m_Dimensions.x, m_Dimensions.y);
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
	return fmt::format("GLViewport pos: {} size: {}", m_coord, m_Dimensions);
}

}}} // namespace GLEngine::GLRenderer::Commands