#include <RendererStdafx.h>

#include <Renderer/Textures/TextureView.h>

namespace GLEngine::Renderer {

//=================================================================================
C_TextureView::C_TextureView(I_TextureViewStorage* storage)
	: m_Storage(storage)
{
}

//=================================================================================
std::size_t C_TextureView::GetAddress(const glm::ivec2& uv) const
{
	const auto dim = m_Storage->GetDimensions();
	return (static_cast<std::size_t>(dim.x) * uv.y + uv.x) * m_Storage->GetNumElements();
}

} // namespace GLEngine::Renderer
