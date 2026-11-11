#pragma once


#include <DULib/StrongType.h>

namespace GLEngine::Physics {
namespace Scalars {
using distance = DULib::StrongType<float, struct distance_tag>; // m

//=================================================================================
// distance
//=================================================================================
std::strong_ordering order(distance, distance);

bool	 equals(distance, distance);
distance add(distance, distance);
distance subtract(distance, distance);
distance divide(distance, float);
distance multiply(distance, float);
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
auto add(position, d_position) -> position;
auto subtract(position, d_position) -> position;

inline float getDistance(const position& p1, const position& p2)
{
	return glm::distance(p1.get(), p2.get());
}

//=================================================================================
// d_position
//=================================================================================
bool equals(d_position, float);
} // namespace Phys2D
namespace Phys3D {
using position	 = DULib::StrongType<glm::vec3, struct position_3d_tag>;
using d_position = DULib::StrongType<glm::vec3, struct d_position_3d_tag>;
}
} // namespace GLEngine::Physics