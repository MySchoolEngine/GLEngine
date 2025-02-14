#include <RendererStdafx.h>

#include <Renderer/Textures/TextureView.h>

#include <glm/gtc/integer.hpp>

namespace GLEngine::Renderer {

//=================================================================================
C_TextureView::C_TextureView(I_TextureViewStorage* storage)
	: m_Storage(storage)
	, m_BorderColor(1, 0, 1, 0)
	, m_Rect(0, 0, storage ? storage->GetDimensions().x : 0, storage ? storage->GetDimensions().y : 0)
	, m_EnableBlending(false)
	, m_BlendOperation(E_BlendFunction::Add)
	, m_WrapFunction(E_WrapFunction::Repeat) // todo check if correct
{
}

//=================================================================================
void C_TextureView::FillLineSpan(const Colours::T_Colour& colour, unsigned int line, unsigned int start, unsigned int end)
{
	if (line > 0 && line < m_Storage->GetDimensions().y - 1)
		m_Storage->FillLineSpan(colour, line, std::max(start, 0u), std::min(end, m_Storage->GetDimensions().x - 1));
}

//=================================================================================
glm::vec2 C_TextureView::GetUVForPixel(const glm::uvec2& coord) const
{
	const auto onePixel = glm::vec2{1.f, 1.f} / glm::vec2{GetDimensions()};
	float	   u		= coord.x * onePixel.x;
	float	   v		= 1.f - (coord.y * onePixel.y);
	return glm::vec2{u, v} + glm::vec2{onePixel.x * 0.5f, -onePixel.y * 0.5f};
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
	float u = (uv.x * (static_cast<float>(m_Rect.GetWidth())));
	float v = ((1 - uv.y) * static_cast<float>(m_Rect.GetHeight()));
	if (uv.x == 1.f)
		u = static_cast<float>(m_Rect.Right());
	if (uv.y == 0.f)
		v = static_cast<float>(m_Rect.Bottom());
	return {std::floor(u), std::floor(v)};
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
const Core::S_Rect& C_TextureView::GetRect() const
{
	return m_Rect;
}

//=================================================================================
void C_TextureView::EnableBlending(bool enable)
{
	m_EnableBlending = enable;
}

//=================================================================================
bool C_TextureView::UseBorderColor() const
{
	return m_WrapFunction == E_WrapFunction::ClampToBorder;
}

//=================================================================================
glm::uvec2 C_TextureView::ClampCoordinates(const glm::ivec2& uv) const
{
	const glm::ivec2 dim	= m_Rect.GetSize();
	glm::ivec2		 result = uv;
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
class BlendFunctionFactory {
public:
	static const std::function<glm::vec3(const glm::vec3&, const glm::vec3&)> GetBlendFunction(E_BlendFunction function)
	{
		switch (function)
		{
		case GLEngine::Renderer::E_BlendFunction::Add:
			return [](const glm::vec3& dst, const glm::vec3& src) { return src + dst; };
		case GLEngine::Renderer::E_BlendFunction::Subtract:
			return [](const glm::vec3& dst, const glm::vec3& src) { return src - dst; };
		case GLEngine::Renderer::E_BlendFunction::ReverseSubtract:
			return [](const glm::vec3& dst, const glm::vec3& src) { return dst - src; };
		case GLEngine::Renderer::E_BlendFunction::Min:
			return [](const glm::vec3& dst, const glm::vec3& src) { return glm::min(dst, src); };
		case GLEngine::Renderer::E_BlendFunction::Max:
			return [](const glm::vec3& dst, const glm::vec3& src) { return glm::max(dst, src); };
		default:
			GLE_ASSERT(false, "Unknown blend function.");
			break;
		}
		return [](const glm::vec3& dst, const glm::vec3& src) { return src + dst; };
	}
};

//=================================================================================
void C_TextureView::DrawPixel(const glm::uvec2& coord, glm::vec4&& colour)
{
	if (!m_EnableBlending || colour.a >= 1.f)
	{
		Set(coord, std::move(colour));
	}
	else
	{
		const auto currentCol = Get<glm::vec4>(coord);
		const auto alpha	  = colour.a;
		Set(coord, glm::vec4(BlendFunctionFactory::GetBlendFunction(m_BlendOperation)(glm::vec3(currentCol) * (1.f - alpha), glm::vec3{colour} * alpha), 1.f));
	}
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
	: m_Size(size)
	, m_View(view)
{
}

} // namespace GLEngine::Renderer
