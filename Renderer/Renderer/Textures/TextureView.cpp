#include <RendererStdafx.h>

#include <Renderer/Textures/TextureView.h>

namespace GLEngine::Renderer {

//=================================================================================
C_TextureView::C_TextureView(I_TextureViewStorage* storage)
	: m_Storage(storage)
	, m_BorderColor(1,0,1,0)
{
}

//=================================================================================
std::size_t C_TextureView::GetAddress(const glm::ivec2& uv) const
{
	const auto dim = m_Storage->GetDimensions();
	return (static_cast<std::size_t>(dim.x) * uv.x + uv.y) * m_Storage->GetNumElements();
}

//=================================================================================
glm::vec2 C_TextureView::GetPixelCoord(const glm::vec2& uv) const
{
	const auto dim = m_Storage->GetDimensions() - glm::ivec2(1, 1);
	return uv * glm::vec2(dim);
}

//=================================================================================
bool C_TextureView::IsOutsideBorders(const glm::vec2& uv) const
{
	const auto dim = m_Storage->GetDimensions();
	return (uv.x < 0 || uv.y < 0 || uv.x >= dim.x || uv.y >= dim.y);
}

} // namespace GLEngine::Renderer
