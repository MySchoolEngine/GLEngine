#pragma once

#include <Renderer/Textures/TextureView.h>

namespace GLEngine::Renderer {
//=================================================================================
struct T_Nearest {
	template <class T> [[nodiscard]] auto FilteredGet(const glm::vec2& pixelCoord, const C_TextureView& view, E_TextureChannel channel) const;
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

//=================================================================================
template <class T> [[nodiscard]] inline auto T_Bilinear::FilteredGet(const glm::vec2& pixelCoord, const C_TextureView& view, E_TextureChannel channel) const
{
	const auto weights		= glm::fract(pixelCoord);
	const auto leftTopCoord = glm::ivec2(pixelCoord - weights);
	const auto Q11			= view.Get<T>(leftTopCoord, channel);
	const auto Q12			= view.Get<T>(leftTopCoord + glm::ivec2(0, 1), channel);
	const auto Q21			= view.Get<T>(leftTopCoord + glm::ivec2(1, 0), channel);
	const auto Q22			= view.Get<T>(leftTopCoord + glm::ivec2(1, 1), channel);

	const auto R1 = glm::mix(Q11, Q21, weights.x);
	const auto R2 = glm::mix(Q12, Q22, weights.x);

	const auto result = glm::mix(R1, R2, weights.y);

	return result;
}

} // namespace GLEngine::Renderer