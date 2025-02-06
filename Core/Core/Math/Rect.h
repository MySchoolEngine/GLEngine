#pragma once

#include <Core/CoreApi.h>

#include <Physics/2D/Plane2D.h>

#include <glm/vec2.hpp>

#include <algorithm>
#include <rttr/registration.h>
#include <rttr/registration_friend.h>

namespace GLEngine::Core {

/**
 * Represent rectangle inside of an image. In order to be valid needs to have width and height > 0.
 *
 * Top < Bottom
 * Left < Right
 */
struct CORE_API_EXPORT S_Rect final {
public:
	constexpr S_Rect()
		: x(0)
		, y(0)
		, width(0)
		, height(0)
	{
	}
	constexpr S_Rect(int x, int y, unsigned int width, unsigned int height)
		: x(x)
		, y(y)
		, width(width)
		, height(height)
	{
	}

	[[nodiscard]] constexpr S_Rect GetIntersection(const S_Rect& other) const
	{
		const int top  = std::max(y, other.y);
		const int left = std::max(x, other.x);
		return {top, left, std::min(Bottom(), other.Bottom()) - top + 1, std::min(Right(), other.Right()) - left + 1};
	}

	/**
	 * @brief This function incorrectly counts on Top > Bottom but holds Left < Right
	 * This behaviour should be fixed one day, but physical world 2D relies on that.
	 *
	 * @param origin		- Origin of the 2d Plane. Have to be inside the rect.
	 * @param direction		- It doesn't matter the orientation as long as it is tangent of the plane
	 * @param intersects	- Return of the intersections
	 * @return true if intersection happens
	 */
	[[nodiscard]] bool IntersectionPlane(const glm::vec2& origin, const glm::vec2& direction, std::array<glm::ivec2, 2>& intersects) const;

	[[nodiscard]] constexpr bool Contains(const glm::vec2& point) const { return point.x > x && point.y > y && (point.x - x) < width && (point.y - y) < height; }
	[[nodiscard]] constexpr bool Contains(const glm::ivec2& point) const
	{
		if (std::signbit(point.x) || std::signbit(point.y))
			return false; // smaller than 0
		glm::uvec2 uPoint = point;
		return uPoint.x >= x && uPoint.y >= y && (uPoint.x - x) < width && (uPoint.y - y) < height;
	}

	constexpr glm::uvec2 TopLeft() const { return		glm::uvec2(Left() ,Top()); }
	constexpr glm::uvec2 TopRight() const { return		glm::uvec2(Right(), Top()); }
	constexpr glm::uvec2 BottomLeft() const { return	glm::uvec2(Left(), Bottom()); }
	constexpr glm::uvec2 BottomRight() const { return	glm::uvec2(Right(),Bottom()); }

	constexpr unsigned int Top() const { return y; }
	constexpr unsigned int Bottom() const { return y + height - 1; }
	constexpr unsigned int Left() const { return x; }
	constexpr unsigned int Right() const { return x + width - 1; }

	constexpr glm::uvec2   GetSize() const { return {width, height}; }
	constexpr unsigned int GetArea() const { return width * height; }

	constexpr unsigned int GetWidth() const { return width; }
	constexpr unsigned int GetHeight() const { return height; }

	[[nodiscard]] constexpr bool IsValid() const { return (x >= 0) && (y >= 0) && (width > 0 || height > 0); }

private:
	unsigned int x, y;
	unsigned int width, height;

	RTTR_ENABLE();
	RTTR_REGISTRATION_FRIEND;
};

} // namespace GLEngine::Core