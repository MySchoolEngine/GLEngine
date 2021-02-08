#pragma once

#include <Renderer/Textures/TextureView.h>

namespace GLEngine::Renderer {
//=================================================================================
struct T_Nearest {
	template <class T> [[nodiscard]] 
	auto FilteredGet(const glm::vec2& pixelCoord, const C_TextureView& view, E_TextureChannel channel) const;
};

//=================================================================================
struct T_Bilinear {
	template <class T> [[nodiscard]] auto FilteredGet(const glm::vec2& pixelCoord, const C_TextureView& view, E_TextureChannel channel) const;
};

//=================================================================================
struct T_Bicubic {
	template <class T> [[nodiscard]] auto FilteredGet(const glm::vec2& pixelCoord, const C_TextureView& view, E_TextureChannel channel) const;
};

//=================================================================================
template <class T> [[nodiscard]] inline auto T_Nearest::FilteredGet(const glm::vec2& pixelCoord, const C_TextureView& view, E_TextureChannel channel) const
{
	return view.Get<T>(glm::ivec2(glm::round(pixelCoord)), channel);
}

} // namespace GLEngine::Rendere