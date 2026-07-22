#pragma once

#include <Utils/SSE/Vec3.h>

#include <glm/glm.hpp>

#include <gtest/gtest.h>

namespace Utils::SSE {

inline std::ostream& operator<<(std::ostream& stream, const Vec3& vec)
{
	return stream << "Utils::SSE::Vec3(" << vec.x() << ", " << vec.y() << ", " << vec.z() << ")";
}
} // namespace Utils::SSE

template <glm::length_t L, class RawType>
testing::AssertionResult AssertVecAlmostEq(const char*								  lhs_expression,
										   const char*								  rhs_expression,
										   const glm::vec<L, RawType, glm::defaultp>& lhs_value,
										   const glm::vec<L, RawType, glm::defaultp>& rhs_value)
{
	bool allAlmostEqual = true;
	for (glm::length_t i = 0; i < L; ++i)
	{
		const testing::internal::FloatingPoint<RawType> lhs_i(lhs_value[i]), rhs_i(rhs_value[i]);
		if (!lhs_i.AlmostEquals(rhs_i))
		{
			allAlmostEqual = false;
			break;
		}
	}
	if (allAlmostEqual)
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

// Component-wise near-equality with an explicit absolute-error tolerance, for values that legitimately
// carry floating-point rounding noise (e.g. results of sin/cos) and so cannot be expected to match
// bit-for-bit or within a few ULPs the way AssertVecAlmostEq requires.
template <glm::length_t L, class RawType>
testing::AssertionResult AssertVecNear(const char*								  lhs_expression,
									   const char*								  rhs_expression,
									   const char*								  abs_error_expression,
									   const glm::vec<L, RawType, glm::defaultp>& lhs_value,
									   const glm::vec<L, RawType, glm::defaultp>& rhs_value,
									   RawType									  abs_error)
{
	bool allNear = true;
	for (glm::length_t i = 0; i < L; ++i)
	{
		if (std::abs(lhs_value[i] - rhs_value[i]) > abs_error)
		{
			allNear = false;
			break;
		}
	}
	if (allNear)
	{
		return testing::AssertionSuccess();
	}

	::std::stringstream lhs_ss;
	lhs_ss.precision(std::numeric_limits<RawType>::digits10 + 2);
	lhs_ss << lhs_value;

	::std::stringstream rhs_ss;
	rhs_ss.precision(std::numeric_limits<RawType>::digits10 + 2);
	rhs_ss << rhs_value;

	return testing::AssertionFailure() << "The difference between " << lhs_expression << " and " << rhs_expression << " exceeds " << abs_error_expression << ", where\n"
									   << lhs_expression << " evaluates to " << testing::internal::StringStreamToString(&lhs_ss) << ",\n"
									   << rhs_expression << " evaluates to " << testing::internal::StringStreamToString(&rhs_ss) << ", and\n"
									   << abs_error_expression << " evaluates to " << abs_error << ".";
}

template <class RawType>
testing::AssertionResult AssertSSEVec3AlmostEq(const char* lhs_expression, const char* rhs_expression, const ::Utils::SSE::Vec3& lhs_value, const ::Utils::SSE::Vec3& rhs_value)
{
	const testing::internal::FloatingPoint<RawType> lhs_x(lhs_value.x()), lhs_y(lhs_value.y()), lhs_z(lhs_value.z()), rhs_x(rhs_value.x()), rhs_y(rhs_value.y()),
		rhs_z(rhs_value.z());
	if (lhs_x.AlmostEquals(rhs_x) && lhs_y.AlmostEquals(rhs_y) && lhs_z.AlmostEquals(rhs_z))
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
