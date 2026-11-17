#pragma once


#include <DULib/StrongType.h>

namespace GLEngine::Physics {
namespace Scalars {
using distance = DULib::StrongType<float, struct distance_tag>; // m

//=================================================================================
// distance
//=================================================================================
#pragma region-- force
std::strong_ordering order(distance, distance);

bool	 equals(distance, distance);
distance add(distance, distance);
distance subtract(distance, distance);
distance divide(distance, float);
distance multiply(distance, float);

namespace distanceLiterals {
[[nodiscard]] constexpr distance operator""_m(long double meters)
{
	return distance{static_cast<float>(meters)};
}
[[nodiscard]] constexpr distance operator""_cm(long double centimeters)
{
	return distance{static_cast<float>(centimeters) / 100.f};
}
[[nodiscard]] constexpr distance operator""_mm(long double millimeters)
{
	return distance{static_cast<float>(millimeters) / 1000.f};
}
} // namespace distanceLiterals
#pragma endregion

} // namespace Scalars

//=================================================================================
// Phys2D
//=================================================================================
namespace Phys2D {
using position	 = DULib::StrongType<glm::vec2, struct position_2d_tag>;
using d_position = DULib::StrongType<glm::vec2, struct d_position_2d_tag>;

//=================================================================================
// position
//=================================================================================
#pragma region-- force
auto add(position, d_position) -> position;
auto subtract(position, d_position) -> position;
auto multiply(position, float) -> position;

inline Scalars::distance getDistance(const position& p1, const position& p2)
{
	return Scalars::distance{glm::distance(p1.get(), p2.get())};
}
#pragma endregion

//=================================================================================
// d_position
//=================================================================================
bool equals(d_position, d_position);
} // namespace Phys2D
namespace Phys3D {
using position	 = DULib::StrongType<glm::vec3, struct position_3d_tag>;
using d_position = DULib::StrongType<glm::vec3, struct d_position_3d_tag>;
}
} // namespace GLEngine::Physics