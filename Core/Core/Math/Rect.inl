#pragma once

#include <glm/gtx/intersect.hpp>

namespace GLEngine::Core {
inline bool S_Rect::IntersectionPlane(const glm::vec2& origin, const glm::vec2& direction, std::array<glm::ivec2, 2>& intersects) const
{
	if (!Contains(origin))
	{
		return false; // still wrong, but it is minimal requirement for now
	}

	// if x + y != 0 than no intersection
	const glm::uvec2 dimensions{ width, height };
	enum Sides : std::uint8_t
	{
		Top,
		Bottom,
		Right,
		Left
	};
	const std::array sides{
		Physics::Primitives::Plane2D{.Normal = {0, -1}, .Position = {0, dimensions.y - 1}},
		Physics::Primitives::Plane2D{.Normal = {0, 1}, .Position = {0, 0}}, // texture is flipped upside down
		Physics::Primitives::Plane2D{.Normal = {-1, 0}, .Position = {dimensions.x - 1, 0}},
		Physics::Primitives::Plane2D{.Normal = {1, 0}, .Position = {0, 0}},
	};
	const auto intersectSide = [planeOrig = origin, &dimensions](const Physics::Primitives::Plane2D& plane, float& distance, const glm::vec2& direction) -> bool {
		return glm::intersectRayPlane(planeOrig, direction, plane.Position, plane.Normal, distance);
		};
	float distance;
	// I need two intersects
	// x horizontal
	// y vertical

	{
		bool hit = false;
		auto currDir = direction;
		if (currDir.y > 0)
			currDir = -currDir;
		// have to hit bottom or one of the sides
		if (intersectSide(sides[Bottom], distance, currDir))
		{
			intersects[0] = origin + currDir * distance;
			//GLE_ASSERT(intersects[0].y == 0, "Should be 0");
			intersects[0].y = 0;
			if (Contains(intersects[0]))
				hit = true;
		}
		if (!hit)
		{
			if (currDir.x < 0)
			{
				// have to hit bottom or left
				if (intersectSide(sides[Left], distance, currDir))
				{
					intersects[0] = origin + currDir * distance;
					//GLE_ASSERT(intersects[0].x == 0, "Should be 0");
					intersects[0].x = 0;
				}
				else
				{
					//GLE_ERROR("Missing intersection");
				}
			}
			else
			{
				// have to hit bottom or right
				if (intersectSide(sides[Right], distance, currDir))
				{
					intersects[0] = origin + currDir * distance;
					intersects[0].x = dimensions.x - 1;
				}
				else
				{
					//GLE_ERROR("Missing intersection");
				}
			}
		}
	}

	{
		bool hit = false;
		auto currDir = direction;
		if (currDir.y < 0)
			currDir = -currDir;
		// have to hit top or one of the sides
		if (intersectSide(sides[Top], distance, currDir))
		{
			intersects[1] = origin + currDir * distance;
			intersects[1].y = dimensions.y - 1;
			if (Contains(intersects[1]))
				hit = true;
		}
		if (!hit)
		{
			if (currDir.x < 0)
			{
				// have to hit bottom or left
				if (intersectSide(sides[Left], distance, currDir))
				{
					intersects[1] = origin + currDir * distance;
					//GLE_ASSERT(intersects[1].x == 0, "Should be 0");
					intersects[1].x = 0;
				}
				else
				{
					//GLE_ERROR("Missing intersection");
				}
			}
			else
			{
				// have to hit bottom or right
				if (intersectSide(sides[Right], distance, currDir))
				{
					intersects[1] = origin + currDir * distance;
					intersects[1].x = dimensions.x - 1;
				}
				else
				{
					//GLE_ERROR("Missing intersection");
				}
			}
		}
	}

	return true;
}

} // namespace GLEngine::Core