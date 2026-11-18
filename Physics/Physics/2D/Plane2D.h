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
	/**
	 * @brief 
	 * @param point 
	 * @return if > 0 the point is on same side as Normal, if == 0 exactly on the plane
	 */
	[[nodiscard]] float DistanceToLineSigned(const glm::vec2& point) const { return Normal.x * (point.x - Position.x) + Normal.y * (point.y - Position.y); }

	[[nodiscard]] constexpr glm::vec2 GetDirection() const { return {Normal.y, -Normal.x}; }
};
} // namespace GLEngine::Physics::Primitives