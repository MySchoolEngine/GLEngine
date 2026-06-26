#pragma once

#include <Renderer/Textures/TextureView.h>

namespace GLEngine::Renderer {
/**
 * Gets pixel coordinate with fractional part to sample filtered value according to given
 * filtering function.
 */

//=================================================================================
struct T_Nearest {
	template <class T> [[nodiscard]] auto FilteredGet(const glm::vec2& pixelCoord, const C_TextureView& view, E_TextureChannel channel) const;
	template <class T, typename = std::enable_if_t<glm::type<T>::is_vec>> [[nodiscard]] auto FilteredGet(const glm::vec2& pixelCoord, const C_TextureView& view) const;
};

//=================================================================================
struct T_Bilinear {
	template <class T> [[nodiscard]] auto FilteredGet(const glm::vec2& pixelCoord, const C_TextureView& view, E_TextureChannel channel) const;
	template <class T, typename = std::enable_if_t<glm::type<T>::is_vec>> [[nodiscard]] auto FilteredGet(const glm::vec2& pixelCoord, const C_TextureView& view) const;
};

//=================================================================================
struct T_Bicubic {
	template <class T> [[nodiscard]] auto FilteredGet(const glm::vec2& pixelCoord, const C_TextureView& view, E_TextureChannel channel) const;
};

//=================================================================================
template <class T> [[nodiscard]] inline auto T_Nearest::FilteredGet(const glm::vec2& pixelCoord, const C_TextureView& view, E_TextureChannel channel) const
{
	return view.Get<T>(glm::uvec2(glm::floor(pixelCoord)), channel);
}

//=================================================================================
template <class T, typename> auto T_Nearest::FilteredGet(const glm::vec2& pixelCoord, const C_TextureView& view) const
{
	return view.Get<T>(glm::ivec2(glm::floor(pixelCoord)));
}

//=================================================================================
template <class T> [[nodiscard]] inline auto T_Bilinear::FilteredGet(const glm::vec2& pixelCoord, const C_TextureView& view, E_TextureChannel channel) const
{
	const auto weights		= glm::fract(pixelCoord);
	const auto leftTopCoord = glm::uvec2(pixelCoord - weights);
	const auto Q11			= view.Get<T>(leftTopCoord, channel);
	const auto Q12			= view.Get<T>(leftTopCoord + glm::uvec2(0, 1), channel);
	const auto Q21			= view.Get<T>(leftTopCoord + glm::uvec2(1, 0), channel);
	const auto Q22			= view.Get<T>(leftTopCoord + glm::uvec2(1, 1), channel);

	const auto R1 = glm::mix(Q11, Q21, weights.x);
	const auto R2 = glm::mix(Q12, Q22, weights.x);

	const auto result = glm::mix(R1, R2, weights.y);

	return result;
}

//=================================================================================
template <class T, typename> auto T_Bilinear::FilteredGet(const glm::vec2& pixelCoord, const C_TextureView& view) const
{
	auto fract		  = glm::fract(pixelCoord);
	auto weights	  = glm::vec2(1.5f) - fract;
	auto leftTopCoord = glm::ivec2(glm::floor(pixelCoord));

	if (fract.x < 0.5f)
	{
		leftTopCoord.x--;
		weights.x -= 1.f;
	}
	if (fract.y < 0.5f)
	{
		leftTopCoord.y--;
		weights.y -= 1.f;
	}

	const auto Q11 = view.Get<T>(leftTopCoord);
	const auto Q12 = view.Get<T>(leftTopCoord + glm::ivec2(0, 1));
	const auto Q21 = view.Get<T>(leftTopCoord + glm::ivec2(1, 0));
	const auto Q22 = view.Get<T>(leftTopCoord + glm::ivec2(1, 1));

	const auto R1 = glm::mix(Q21, Q11, weights.x);
	const auto R2 = glm::mix(Q22, Q12, weights.x);

	const auto result = glm::mix(R2, R1, weights.y);

	return result;
}

} // namespace GLEngine::Renderer