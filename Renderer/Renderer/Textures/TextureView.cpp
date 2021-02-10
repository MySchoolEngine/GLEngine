#include <RendererStdafx.h>

#include <Renderer/Textures/TextureView.h>

#include <glm/gtc/integer.hpp>

namespace GLEngine::Renderer {

//=================================================================================
C_TextureView::C_TextureView(I_TextureViewStorage* storage)
	: m_Storage(storage)
	, m_BorderColor(1,0,1,0)
	, m_WrapFunction(E_WrapFunction::Repeat)
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

//=================================================================================
void C_TextureView::SetBorderColor(const glm::vec4& color)
{
	m_BorderColor = color;
}

//=================================================================================
E_WrapFunction C_TextureView::GetWrapFunction() const
{
	return m_WrapFunction;
}

//=================================================================================
void C_TextureView::SetWrapFunction(E_WrapFunction wrap)
{
	m_WrapFunction = wrap;
}

//=================================================================================
bool C_TextureView::UseBorderColor() const
{
	return m_WrapFunction == E_WrapFunction::ClampToBorder;
}

//=================================================================================
glm::ivec2 C_TextureView::ClampCoordinates(const glm::ivec2& uv) const
{
	const auto dim	  = m_Storage->GetDimensions() - glm::ivec2(1, 1);
	glm::ivec2 result = uv;
	switch (m_WrapFunction)
	{
	case E_WrapFunction::ClampToEdge:
		result = glm::clamp(result, {0, 0}, dim);
		break;
	case E_WrapFunction::Repeat:
		result = uv % dim;
		break;
	case E_WrapFunction::MirroredRepeat:
		CORE_LOG(E_Level::Error, E_Context::Render, "Unimplemented");
		break;
	case E_WrapFunction::ClampToBorder:
	default:
		CORE_LOG(E_Level::Error, E_Context::Render, "Unsupported");
		break;
	}

	return result;
}

//=================================================================================
const I_TextureViewStorage* const C_TextureView::GetStorage() const
{
	return m_Storage;
}

} // namespace GLEngine::Renderer
