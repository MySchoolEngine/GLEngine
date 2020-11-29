#pragma once

#include <Renderer/RendererApi.h>
#include <Renderer/Textures/TextureStorage.h>
/**
	*	This class serves as CPU side view to the texture independently whether lies on
	*	CPU or GPU memory.
	*
	*	Problem breaks up into three sub-problems:
	*	1]	Storage (cpu, gpu, fixed, etc)
	*	2]	Internal format
	*	3]	Conversion into external format
	*
	*	Storage
	*	We can store texture either on CPU or GPU side.
	*	GPU - we can either download data or access them directly.
	*	CPU - we can use some STD container
	*
	*	Internal format
	*	Formats differs in three orthogonal axes.
	*	1]	Number of elements
	*	2]	Precision
	*	3]	Floating point integer formats
	*
	*	Conversion to external format
	*	
	*
	*
	*
	*	Let's start with simple part CPU side let's say GL_RGB8 format
	*/

namespace GLEngine::Renderer
{

//=================================================================================
class RENDERER_API_EXPORT C_TextureView
{
public:
	C_TextureView(I_TextureViewStorage* storage);
	template<class T>
	[[nodiscard]] T Get(const glm::ivec2& uv, E_TextureElement element) const;

	// implement bilinear filtering
	//template<class T>
	//[[nodiscard]] T Get(const glm::vec2& uv) const;

	template<class T>
	void Set(const glm::ivec2& uv, const T val, E_TextureElement element)
	{
		const auto dim = m_Storage->GetDimensions();
		if (uv.x < 0 || uv.x > dim.x || uv.y < 0 || uv.y > dim.y)
		{
			CORE_LOG(E_Level::Info, E_Context::Render, "Writing outside of texture buffer. Result would be discarded.");
			return;
		}

		m_Storage->Set(val, GetAddress(uv) + m_Storage->GetElementOffset(element));
	}
private:
	[[nodiscard]] std::size_t GetAddress(const glm::ivec2& uv) const;

	I_TextureViewStorage*	m_Storage; // not owning ptr
};

template<>
inline std::uint8_t C_TextureView::Get<std::uint8_t>(const glm::ivec2& uv, E_TextureElement element) const
{
	return m_Storage->GetI(GetAddress(uv) + m_Storage->GetElementOffset(element));
}
}
