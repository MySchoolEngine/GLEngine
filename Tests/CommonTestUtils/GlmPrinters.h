#pragma once

#include <gtest/gtest.h>

#include <glm/glm.hpp>

template <class RawType>
testing::AssertionResult AssertVec2AlmostEq(const char*								   lhs_expression,
											const char*								   rhs_expression,
											const glm::vec<2, RawType, glm::defaultp>& lhs_value,
											const glm::vec<2, RawType, glm::defaultp>& rhs_value)
{
	const testing::internal::FloatingPoint<RawType> lhs_x(lhs_value.x), lhs_y(lhs_value.y), rhs_x(rhs_value.x), rhs_y(rhs_value.y);
	if (lhs_x.AlmostEquals(rhs_x) && lhs_y.AlmostEquals(rhs_y))
	{
		return testing::AssertionSuccess();
	}

	::std::stringstream lhs_ss;
	lhs_ss.precision(std::numeric_limits<RawType>::digits10 + 2);
	lhs_ss << lhs_value;

	::std::stringstream rhs_ss;
	rhs_ss.precision(std::numeric_limits<RawType>::digits10 + 2);
	rhs_ss << rhs_value;

	return testing::internal::EqFailure(lhs_expression, rhs_expression, testing::internal::StringStreamToString(&lhs_ss), testing::internal::StringStreamToString(&rhs_ss), false);
}

namespace glm {

inline std::ostream& operator<<(std::ostream& stream, const vec2& vec)
{
	return stream << "glm::vec3(" << vec.x << ", " << vec.y << ")";
}

inline std::ostream& operator<<(std::ostream& stream, const vec3& vec)
{
	return stream << "glm::vec3(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
}

inline std::ostream& operator<<(std::ostream& stream, const vec4& vec)
{
	return stream << "glm::vec4(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")";
}

inline std::ostream& operator<<(std::ostream& stream, const uvec2& vec)
{
	return stream << "glm::uvec2(" << vec.x << ", " << vec.y << ")";
}
} // namespace glm
