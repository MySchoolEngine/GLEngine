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

#include <glm/gtx/type_trait.hpp>

namespace GLEngine::Renderer
{

//=================================================================================
class RENDERER_API_EXPORT C_TextureView {
public:
	C_TextureView(I_TextureViewStorage* storage);
	template <class T> [[nodiscard]] T Get(const glm::ivec2& uv, E_TextureChannel element) const;

	// implement bilinear filtering
	// template<class T>
	//[[nodiscard]] T Get(const glm::vec2& uv) const;

	template <class T> void Set(const glm::ivec2& uv, const T val, E_TextureChannel element)
	{
		const auto dim = m_Storage->GetDimensions();
		if (uv.x < 0 || uv.x > dim.x || uv.y < 0 || uv.y > dim.y)
		{
			CORE_LOG(E_Level::Info, E_Context::Render, "Writing outside of texture buffer. Result would be discarded.");
			return;
		}

		m_Storage->Set(val, GetAddress(uv) + m_Storage->GetChannelOffset(element));
	}

	template<class T, typename = std::enable_if_t<glm::type<T>::is_vec>>
	void Set(const glm::ivec2& uv, T&& val)
	{
		// TODO set it as whole vector if storage is not swizzled
		for (std::uint8_t i = 0; i < std::min(static_cast<std::uint8_t>(glm::type<T>::components), m_Storage->GetNumElements()); ++i)
		{
			Set(uv, val[i], static_cast<E_TextureChannel>(i));
		}
	}

	void ClearColor(const glm::vec4& colour)
	{
		const auto dim = m_Storage->GetDimensions();
		// TODO set it as whole vector if storage is not swizzled
		// or swizzle this and memset it all over storage
		for (int u = 0; u < dim.x; ++u)
		{
			for (int v = 0; v < dim.y; ++v)
			{
				const glm::ivec2 uv{ u, v };
				Set(uv, glm::vec4(colour));
			}
		}
	}

	template<class T, typename = std::enable_if_t<glm::type<T>::is_vec>>
	T Get(const glm::ivec2& uv) const
	{
		T ret;
		for (std::uint8_t i = 0; i < std::min(static_cast<std::uint8_t>(glm::type<T>::components), m_Storage->GetNumElements()); ++i)
		{
			ret[i] = Get<typename T::value_type>(uv, static_cast<E_TextureChannel>(i));
		}
		return ret;
	}
protected:
	[[nodiscard]] std::size_t GetAddress(const glm::ivec2& uv) const;

	I_TextureViewStorage* m_Storage; // not owning ptr
};

template <> inline std::uint8_t C_TextureView::Get<std::uint8_t>(const glm::ivec2& uv, E_TextureChannel element) const
{
	return m_Storage->GetI(GetAddress(uv) + m_Storage->GetChannelOffset(element));
}

template<>
inline float C_TextureView::Get<float>(const glm::ivec2& uv, E_TextureChannel element) const
{
	return m_Storage->GetF(GetAddress(uv) + m_Storage->GetChannelOffset(element));
}
  

inline glm::vec2 signNotZero(glm::vec2 v) {
	return glm::vec2((v.x >= 0.0) ? +1.0 : -1.0, (v.y >= 0.0) ? +1.0 : -1.0);
}

inline glm::vec2 float32x3_to_oct(glm::vec3 v) {
	// Project the sphere onto the octahedron, and then onto the xy plane
	glm::vec2 p = glm::vec2{ v.x, v.y } * (1.0f / (abs(v.x) + abs(v.y) + abs(v.z)));
	// Reflect the folds of the lower hemisphere over the diagonals
	return (v.z <= 0.0f) ? ((1.0f - abs(glm::vec2{ p.y, p.x })) * signNotZero(p)) : p;
}

// Represents texture view as described in 
// @source:	http://jcgt.org/published/0003/02/01/paper.pdf
// @name:	 A Survey of Efficient Representations for Independent Unit Vectors
// Expect texture view of size at least equal to @param size.
// @param size defines square area of texture where should be the resulting octahedron mapping
class RENDERER_API_EXPORT C_OctahedralTextureView
{
public:
	C_OctahedralTextureView(C_TextureView* view, std::size_t size)
		: m_View(view)
		, m_Size(size) 
	{}

	template<class T>
	void Set(const glm::vec3& direction, const T val, E_TextureChannel element)
	{
		const auto coord = float32x3_to_oct(direction);
		const auto realCoord = glm::ivec2{ static_cast<float>(m_Size) * (coord / 2.0f + 0.5f) };

		m_View->Set(realCoord, val, element);
	}
private:
	std::size_t		m_Size;
	C_TextureView*	m_View;
};

} // namespace GLEngine::Renderer
