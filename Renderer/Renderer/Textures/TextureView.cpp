#include <RendererStdafx.h>

#include <Renderer/Textures/TextureView.h>

#include <glm/gtc/integer.hpp>

namespace GLEngine::Renderer {

//=================================================================================
C_TextureView::C_TextureView(I_TextureViewStorage* storage)
	: m_Storage(storage)
	, m_BorderColor(1, 0, 1, 0)
	, m_WrapFunction(E_WrapFunction::Repeat)
{
}

//=================================================================================
std::size_t C_TextureView::GetAddress(const glm::ivec2& uv) const
{
	const auto dim = m_Storage->GetDimensions();
	return (static_cast<std::size_t>(dim.x) * uv.y + uv.x) * m_Storage->GetNumElements();
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
	const auto dim = m_Storage->GetDimensions();
	// TODO set it as whole vector if storage is not swizzled
	// or swizzle this and memset it all over storage
	for (int u = 0; u < dim.x; ++u)
	{
		for (int v = 0; v < dim.y; ++v)
		{
			const glm::ivec2 uv{u, v};
			Set(uv, glm::vec4(colour));
		}
	}
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
