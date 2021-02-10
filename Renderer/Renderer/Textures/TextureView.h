#pragma once

#include <Renderer/RendererApi.h>
#include <Renderer/Textures/TextureStorage.h>
#include <Renderer/Textures/TextureDefinitions.h>
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

namespace GLEngine::Renderer {

struct T_Nearest;

//=================================================================================
class RENDERER_API_EXPORT C_TextureView {
public:
	explicit C_TextureView(I_TextureViewStorage* storage);
	template <class T, class Filter = T_Nearest> [[nodiscard]] T Get(const glm::vec2& uv, E_TextureChannel element) const;
	template <class T> [[nodiscard]] T							 Get(const glm::ivec2& uv, E_TextureChannel element) const;
	template <class T> [[nodiscard]] T							 GetBorderColor() const;

	[[nodiscard]] E_WrapFunction GetWrapFunction() const;
	void						 SetWrapFunction(E_WrapFunction wrap);

	template <class T> void Set(const glm::ivec2& uv, const T val, E_TextureChannel element);
	void					SetBorderColor(const glm::vec4& color);
	[[nodiscard]] bool		UseBorderColor() const;

protected:
	[[nodiscard]] std::size_t GetAddress(const glm::ivec2& uv) const;
	[[nodiscard]] glm::vec2	  GetPixelCoord(const glm::vec2& uv) const;
	[[nodiscard]] bool		  IsOutsideBorders(const glm::vec2& uv) const;
	[[nodiscard]] glm::ivec2  ClampCoordinates(const glm::ivec2& uv) const;
	

	I_TextureViewStorage* m_Storage; // not owning ptr
	glm::vec4			  m_BorderColor;
	E_WrapFunction		  m_WrapFunction;
};

template <> glm::vec4  C_TextureView::GetBorderColor() const;
template <> glm::ivec4 C_TextureView::GetBorderColor() const;

// Represents texture view as described in
// @source:	http://jcgt.org/published/0003/02/01/paper.pdf
// @name:	 A Survey of Efficient Representations for Independent Unit Vectors
// Expect texture view of size at least equal to @param size.
// @param size defines square area of texture where should be the resulting octahedron mapping
class RENDERER_API_EXPORT C_OctahedralTextureView {
public:
	C_OctahedralTextureView(C_TextureView* view, std::size_t size)
		: m_View(view)
		, m_Size(size)
	{
	}

	template <class T> void Set(const glm::vec3& direction, const T val, E_TextureChannel element);

private:
	std::size_t	   m_Size;
	C_TextureView* m_View;
};

} // namespace GLEngine::Renderer

#include <Renderer/Textures/TextureView.inl>
