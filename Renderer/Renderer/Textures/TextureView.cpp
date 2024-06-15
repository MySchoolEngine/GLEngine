#include <RendererStdafx.h>

#include <Renderer/Textures/TextureView.h>

#include <glm/gtc/integer.hpp>

namespace GLEngine::Renderer {

//=================================================================================
C_TextureView::C_TextureView(I_TextureViewStorage* storage)
	: m_Storage(storage)
	, m_BorderColor(1, 0, 1, 0)
	, m_WrapFunction(E_WrapFunction::Repeat)
	, m_Rect(0, 0, storage?storage->GetDimensions().x:0, storage?storage->GetDimensions().y:0) // todo check if correct
{
}

//=================================================================================
std::size_t C_TextureView::GetAddress(const glm::uvec2& uv) const
{
	const auto dim = m_Storage->GetDimensions();
	return (static_cast<std::size_t>(dim.x) * uv.y + uv.x) * m_Storage->GetNumElements();
}

//=================================================================================
std::size_t C_TextureView::GetPixelAddress(const glm::uvec2& coord) const
{
	GLE_ASSERT(coord.x < m_Rect.GetWidth() && coord.y < m_Rect.GetHeight(), "Outside of bounds");
	const auto dim			  = m_Storage->GetDimensions();
	const auto addressInImage = coord + glm::uvec2{m_Rect.TopLeft()};
	return (static_cast<std::size_t>(dim.x) * addressInImage.y + addressInImage.x);
}

//=================================================================================
glm::vec2 C_TextureView::GetPixelCoord(const glm::vec2& uv) const
{
	// coord in rect + top left of the rect
	return {(uv.x * (m_Rect.GetWidth())), ((1 - uv.y) * m_Rect.GetHeight())};
}

//=================================================================================
bool C_TextureView::IsOutsideBorders(const glm::ivec2& coord) const
{
	return (coord.x < 0 || coord.y < 0 || coord.x >= static_cast<int>(m_Rect.GetWidth()) || coord.y >= static_cast<int>(m_Rect.GetHeight()));
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
void C_TextureView::SetRect(const Core::S_Rect& rect)
{
	m_Rect = rect.GetIntersection({0, 0, m_Storage ? m_Storage->GetDimensions().x : 0, m_Storage ? m_Storage->GetDimensions().y : 0});
}

//=================================================================================
bool C_TextureView::UseBorderColor() const
{
	return m_WrapFunction == E_WrapFunction::ClampToBorder;
}

//=================================================================================
glm::uvec2 C_TextureView::ClampCoordinates(const glm::ivec2& uv) const
{
	const glm::ivec2 dim  = m_Rect.GetSize();
	glm::ivec2 result = uv;
	switch (m_WrapFunction)
	{
	case E_WrapFunction::ClampToEdge:
		result = glm::clamp(result, {0, 0}, dim);
		break;
	case E_WrapFunction::Repeat: {
		result = uv % dim;
		if (result.x < 0)
		{
			result.x = dim.x + result.x;
		}
		if (result.y < 0)
		{
			result.y = dim.y + result.y;
		}
	}
	break;
	case E_WrapFunction::MirroredRepeat: {
		const auto numRepeats = uv / (dim + glm::ivec2{1, 1});
		result				  = uv - numRepeats * dim;
		// odd repeats -> e.g. second repeat -> mirror
		if (numRepeats.x % 2 == 1)
		{
			result.x = dim.x - result.x;
		}
		if (numRepeats.y % 2 == 1)
		{
			result.y = dim.y - result.y;
		}
	}
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

//=================================================================================
void C_TextureView::ClearColor(const glm::vec4& colour)
{
	// swizzle on view side
	// TODO: should only work for area in rect
	if (m_Rect.TopLeft() == glm::uvec2{0, 0} && m_Rect.GetHeight())
		m_Storage->SetAll(colour);
}

//=================================================================================
const glm::uvec2 C_TextureView::GetDimensions() const
{
	return m_Rect.GetSize();
}

//=================================================================================
// C_OctahedralTextureView
//=================================================================================
C_OctahedralTextureView::C_OctahedralTextureView(C_TextureView view, std::size_t size)
	: m_View(view)
	, m_Size(size)
{
}

} // namespace GLEngine::Renderer
