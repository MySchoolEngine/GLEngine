#pragma once

#include <Physics/GeometryUtils/TriangleIntersect.h>
#include <Physics/Primitives/Ray.h>

#include <expected>
#include <rttr/registration_friend.h>

namespace GLEngine::Physics::Primitives {

struct S_Triangle final {
	enum class CreateError : std::uint8_t
	{
		CollinearPoints,
	};

	[[nodiscard]] static std::expected<S_Triangle, CreateError> Create(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2)
	{
		S_Triangle ret;
		ret.m_p			  = {p0, p1, p2};
		const auto normal = glm::cross(p1 - p0, p2 - p0);
		const auto length = glm::length(normal);
		ret.m_Area		  = length / 2.f;

		// Check for degenerate triangle (collinear points)
		constexpr float EPSILON = 1e-8f;
		if (length <= EPSILON)
		{
			return std::unexpected(CreateError::CollinearPoints);
		}
		ret.m_Normal = glm::normalize(normal);
		return ret;
	}

	[[nodiscard]] float IntersectImpl(const S_Ray& ray, const float tMax) const
	{
		glm::vec2  barycentric;
		const auto distance = TriangleRayIntersect(m_p, ray, &barycentric);
		if (distance < 0 || distance > tMax)
			return distance;


		return distance;
	}

	[[nodiscard]] glm::vec3 GetNormal() const { return m_Normal; }
	[[nodiscard]] float		GetArea() const { return m_Area; }

	std::array<glm::vec3, 3> m_p;

	RTTR_REGISTRATION_FRIEND

private:
	S_Triangle() = default;
	glm::vec3 m_Normal;
	float	  m_Area;
};
} // namespace GLEngine::Physics::Primitives