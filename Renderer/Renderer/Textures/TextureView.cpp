#include <RendererStdafx.h>

#include <Renderer/Textures/TextureView.h>

#include <glm/gtc/integer.hpp>

namespace GLEngine::Renderer {

//=================================================================================
C_TextureView::C_TextureView(I_TextureViewStorage* storage)
	: m_Storage(storage)
	, m_BorderColor(1, 0, 1, 0)
	, m_WrapFunction(E_WrapFunction::Repeat)
	, m_EnableBlending(false)
	, m_BlendOperation(E_BlendFunction::Add)
{
}

//=================================================================================
std::size_t C_TextureView::GetAddress(const glm::ivec2& uv) const
{
	const auto dim = m_Storage->GetDimensions();
	return (static_cast<std::size_t>(dim.x) * uv.y + uv.x) * m_Storage->GetNumElements();
}

//=================================================================================
std::size_t C_TextureView::GetPixelAddress(const glm::uvec2& uv) const
{
	const auto dim = m_Storage->GetDimensions();
	return (static_cast<std::size_t>(dim.x) * uv.y + uv.x);
}

//=================================================================================
glm::vec2 C_TextureView::GetPixelCoord(const glm::vec2& uv) const
{
	const auto dim = m_Storage->GetDimensions() - glm::uvec2(1, 1);
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
glm::uvec2 C_TextureView::ClampCoordinates(const glm::uvec2& uv) const
{
	const auto dim	  = m_Storage->GetDimensions() - glm::uvec2(1, 1);
	glm::uvec2 result = uv;
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
		const auto numRepeats = uv / (dim + glm::uvec2{1, 1});
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
	m_Storage->SetAll(colour);
}

class BlendFunctionFactory {
public:

	static const std::function<glm::vec3(const glm::vec3&, const glm::vec3&)> GetBlendFunction(E_BlendFunction function) {
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
void C_TextureView::DrawPixel(const glm::ivec2& coord, glm::vec4&& colour)
{
	if (!m_EnableBlending) {
		Set(coord, glm::vec3{colour});
	}
	else
	{
		const auto currentCol = Get<glm::vec4>(coord);
		const auto alpha	  = colour.a;
		Set(coord, BlendFunctionFactory::GetBlendFunction(m_BlendOperation)(glm::vec3(currentCol) * (1.f - alpha), glm::vec3{colour} * alpha));
	}
}

//=================================================================================
const glm::uvec2 C_TextureView::GetDimensions() const
{
	return m_Storage->GetDimensions();
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
