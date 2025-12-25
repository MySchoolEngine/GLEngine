#pragma once

#include <glm/glm.hpp>

namespace GLEngine::Colours {

using T_Colour = glm::vec3;

inline constexpr static auto white	= T_Colour(1, 1, 1);
inline constexpr static auto black	= T_Colour(0, 0, 0);
inline constexpr static auto red	= T_Colour(1, 0, 0);
inline constexpr static auto green	= T_Colour(0, 1, 0);
inline constexpr static auto blue	= T_Colour(0, 0, 1);
inline constexpr static auto purple = T_Colour(1, 0, 1);
inline constexpr static auto cyan	= T_Colour(0, 1, 1);
inline constexpr static auto yellow = T_Colour(1, 1, 0);
inline constexpr static auto gray	= T_Colour(169.0 / 255.0, 169.0 / 255.0, 169.0 / 255.0);

namespace Editing {

inline constexpr static auto mouseOver = red;
inline constexpr static auto selected  = blue;
}

} // namespace GLEngine::Colours
