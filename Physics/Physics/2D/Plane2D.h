#pragma once

#include <glm/glm.hpp>

namespace GLEngine::Physics::Primitives {

struct Plane2D {
	glm::vec2			Normal;
	glm::vec2			Position;
	[[nodiscard]] float DistanceToLine(const glm::vec2& point) const
	{
		const auto	dir = GetDirection();
		const auto	v	= point - Position;
		const float d	= glm::dot(v, dir);
		return glm::distance(Position + dir * d, point);
	}

	[[nodiscard]] constexpr glm::vec2 GetDirection() const { return {Normal.y, -Normal.x}; }
};
} // namespace GLEngine::Physics::Primitives