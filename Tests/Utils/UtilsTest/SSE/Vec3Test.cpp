#include <UtilsTestStdafx.h>

#include <Utils/SSE/Vec3.h>

#include <xmmintrin.h>

namespace Utils::SSE {

// =============================================================================
// Construction
// =============================================================================
#pragma region--Vec3_Construction
TEST(Vec3_Construction, DefaultConstructor_IsZero)
{
	const Vec3 v;
	EXPECT_EQ(static_cast<glm::vec3>(v), glm::vec3(0.f));
}

TEST(Vec3_Construction, ScalarConstructor_BroadcastsToXYZ)
{
	const Vec3 v(2.5f);
	EXPECT_EQ(static_cast<glm::vec3>(v), glm::vec3(2.5f));
}

TEST(Vec3_Construction, GlmVec3Constructor_CopiesComponents)
{
	const Vec3 v(glm::vec3(1.f, 2.f, 3.f));
	EXPECT_EQ(static_cast<glm::vec3>(v), glm::vec3(1.f, 2.f, 3.f));
}

TEST(Vec3_Construction, XYZConstructor_SetsComponents)
{
	const Vec3 v(1.f, 2.f, 3.f);
	EXPECT_EQ(static_cast<glm::vec3>(v), glm::vec3(1.f, 2.f, 3.f));
}
#pragma endregion-- Vec3_Construction

// =============================================================================
// Component access
// x()/y()/z() read individual lanes; SetX/SetY/SetZ mutate a single lane in
// place without disturbing the others, including the w lane.
// =============================================================================
#pragma region--Vec3_ComponentAccess
TEST(Vec3_ComponentAccess, Getters_ReturnConstructedComponents)
{
	const Vec3 v(1.f, 2.f, 3.f);
	EXPECT_FLOAT_EQ(v.x(), 1.f);
	EXPECT_FLOAT_EQ(v.y(), 2.f);
	EXPECT_FLOAT_EQ(v.z(), 3.f);
}

TEST(Vec3_ComponentAccess, Getters_IgnoreWLane)
{
	// xyz = (1,2,3), w = 100 must not leak into any getter.
	const Vec3 v(_mm_set_ps(100.f, 3.f, 2.f, 1.f));
	EXPECT_FLOAT_EQ(v.x(), 1.f);
	EXPECT_FLOAT_EQ(v.y(), 2.f);
	EXPECT_FLOAT_EQ(v.z(), 3.f);
}

TEST(Vec3_ComponentAccess, SetX_UpdatesOnlyX)
{
	Vec3 v(1.f, 2.f, 3.f);
	v.SetX(9.f);
	EXPECT_FLOAT_EQ(v.x(), 9.f);
	EXPECT_FLOAT_EQ(v.y(), 2.f);
	EXPECT_FLOAT_EQ(v.z(), 3.f);
}

TEST(Vec3_ComponentAccess, SetY_UpdatesOnlyY)
{
	Vec3 v(1.f, 2.f, 3.f);
	v.SetY(9.f);
	EXPECT_FLOAT_EQ(v.x(), 1.f);
	EXPECT_FLOAT_EQ(v.y(), 9.f);
	EXPECT_FLOAT_EQ(v.z(), 3.f);
}

TEST(Vec3_ComponentAccess, SetZ_UpdatesOnlyZ)
{
	Vec3 v(1.f, 2.f, 3.f);
	v.SetZ(9.f);
	EXPECT_FLOAT_EQ(v.x(), 1.f);
	EXPECT_FLOAT_EQ(v.y(), 2.f);
	EXPECT_FLOAT_EQ(v.z(), 9.f);
}

TEST(Vec3_ComponentAccess, Setters_DoNotAffectWLane)
{
	Vec3 v(_mm_set_ps(42.f, 3.f, 2.f, 1.f)); // xyz = (1,2,3), w = 42
	v.SetX(9.f);
	v.SetY(8.f);
	v.SetZ(7.f);

	alignas(16) float raw[4];
	_mm_store_ps(raw, v.GetRaw());
	EXPECT_FLOAT_EQ(raw[3], 42.f);
}
#pragma endregion-- Vec3_ComponentAccess

// =============================================================================
// operator[]
// Indexed access must match x()/y()/z() and work on const instances.
// =============================================================================
#pragma region--Vec3_IndexOperator
TEST(Vec3_IndexOperator, Index0_ReturnsX)
{
	const Vec3 v(1.f, 2.f, 3.f);
	EXPECT_FLOAT_EQ(v[0], 1.f);
}

TEST(Vec3_IndexOperator, Index1_ReturnsY)
{
	const Vec3 v(1.f, 2.f, 3.f);
	EXPECT_FLOAT_EQ(v[1], 2.f);
}

TEST(Vec3_IndexOperator, Index2_ReturnsZ)
{
	const Vec3 v(1.f, 2.f, 3.f);
	EXPECT_FLOAT_EQ(v[2], 3.f);
}

TEST(Vec3_IndexOperator, MatchesXYZGetters)
{
	const Vec3 v(4.f, 5.f, 6.f);
	EXPECT_FLOAT_EQ(v[0], v.x());
	EXPECT_FLOAT_EQ(v[1], v.y());
	EXPECT_FLOAT_EQ(v[2], v.z());
}

TEST(Vec3_IndexOperator, CallableOnConstReference)
{
	// Compile-time check: operator[] must be usable through a const Vec3&.
	const Vec3	v(1.f, 2.f, 3.f);
	const Vec3& ref = v;
	EXPECT_FLOAT_EQ(ref[0], 1.f);
}

TEST(Vec3_IndexOperator, IgnoresWLaneAtIndices0To2)
{
	// _mm_set_ps(w, z, y, x) — xyz = (1,2,3), w = 100 must not leak into indices 0-2.
	const Vec3 v(_mm_set_ps(100.f, 3.f, 2.f, 1.f));
	EXPECT_FLOAT_EQ(v[0], 1.f);
	EXPECT_FLOAT_EQ(v[1], 2.f);
	EXPECT_FLOAT_EQ(v[2], 3.f);
}
#pragma endregion-- Vec3_IndexOperator

// =============================================================================
// Arithmetic operators
// =============================================================================
#pragma region--Vec3_Arithmetic
TEST(Vec3_Arithmetic, Subtraction_SubtractsComponentwise)
{
	const Vec3 a(5.f, 3.f, 1.f);
	const Vec3 b(1.f, 2.f, 3.f);
	EXPECT_EQ(static_cast<glm::vec3>(a - b), glm::vec3(4.f, 1.f, -2.f));
}

TEST(Vec3_Arithmetic, Addition_AddsComponentwise)
{
	const Vec3 a(1.f, 2.f, 3.f);
	const Vec3 b(4.f, 5.f, 6.f);
	EXPECT_EQ(static_cast<glm::vec3>(a + b), glm::vec3(5.f, 7.f, 9.f));
}

TEST(Vec3_Arithmetic, Multiplication_MultipliesComponentwise)
{
	const Vec3 a(2.f, 3.f, 4.f);
	const Vec3 b(5.f, 6.f, 7.f);
	EXPECT_EQ(static_cast<glm::vec3>(a * b), glm::vec3(10.f, 18.f, 28.f));
}

TEST(Vec3_Arithmetic, Division_DividesComponentwise)
{
	const Vec3 a(10.f, 18.f, 28.f);
	const Vec3 b(2.f, 3.f, 4.f);
	EXPECT_EQ(static_cast<glm::vec3>(a / b), glm::vec3(5.f, 6.f, 7.f));
}

TEST(Vec3_Arithmetic, ScalarMultiplication_ScalesEachComponent)
{
	const Vec3 a(1.f, 2.f, 3.f);
	EXPECT_EQ(static_cast<glm::vec3>(a * 2.f), glm::vec3(2.f, 4.f, 6.f));
}
#pragma endregion-- Vec3_Arithmetic

// =============================================================================
// Equality
// The w lane is not part of the public value — vectors built from raw __m128
// registers with identical xyz but differing w must still compare equal.
// =============================================================================
#pragma region--Vec3_Equality
TEST(Vec3_Equality, IdenticalVectors_AreEqual)
{
	EXPECT_TRUE(Vec3(1.f, 2.f, 3.f) == Vec3(1.f, 2.f, 3.f));
}

TEST(Vec3_Equality, DifferingXComponent_AreNotEqual)
{
	EXPECT_FALSE(Vec3(1.f, 2.f, 3.f) == Vec3(9.f, 2.f, 3.f));
}

TEST(Vec3_Equality, DifferingYComponent_AreNotEqual)
{
	EXPECT_FALSE(Vec3(1.f, 2.f, 3.f) == Vec3(1.f, 9.f, 3.f));
}

TEST(Vec3_Equality, DifferingZComponent_AreNotEqual)
{
	EXPECT_FALSE(Vec3(1.f, 2.f, 3.f) == Vec3(1.f, 2.f, 9.f));
}

TEST(Vec3_Equality, DifferingWLane_IsIgnored)
{
	// _mm_set_ps(w, z, y, x) — both vectors carry xyz = (1,2,3), only w differs.
	const Vec3 a(_mm_set_ps(11.f, 3.f, 2.f, 1.f));
	const Vec3 b(_mm_set_ps(-7.f, 3.f, 2.f, 1.f));
	EXPECT_TRUE(a == b);
}
#pragma endregion-- Vec3_Equality

// =============================================================================
// min / max
// =============================================================================
#pragma region--Vec3_MinMax
TEST(Vec3_MinMax, Min_TakesComponentwiseMinimum)
{
	const Vec3 a(1.f, 5.f, -2.f);
	const Vec3 b(3.f, 2.f, -4.f);
	EXPECT_EQ(static_cast<glm::vec3>(Vec3::min(a, b)), glm::vec3(1.f, 2.f, -4.f));
}

TEST(Vec3_MinMax, Max_TakesComponentwiseMaximum)
{
	const Vec3 a(1.f, 5.f, -2.f);
	const Vec3 b(3.f, 2.f, -4.f);
	EXPECT_EQ(static_cast<glm::vec3>(Vec3::max(a, b)), glm::vec3(3.f, 5.f, -2.f));
}
#pragma endregion-- Vec3_MinMax

// =============================================================================
// MinComponent / MaxComponent
// Horizontal reduction across xyz — the w lane must never influence the result.
// =============================================================================
#pragma region--Vec3_ComponentMinMax
TEST(Vec3_ComponentMinMax, MaxComponent_ReturnsLargestOfXYZ)
{
	const Vec3 v(4.f, 1.f, 7.f);
	EXPECT_FLOAT_EQ(v.MaxComponent(), 7.f);
}

TEST(Vec3_ComponentMinMax, MinComponent_ReturnsSmallestOfXYZ)
{
	const Vec3 v(4.f, 1.f, 7.f);
	EXPECT_FLOAT_EQ(v.MinComponent(), 1.f);
}

TEST(Vec3_ComponentMinMax, MaxComponent_HandlesNegativeComponents)
{
	const Vec3 v(-1.f, -5.f, -3.f);
	EXPECT_FLOAT_EQ(v.MaxComponent(), -1.f);
}

TEST(Vec3_ComponentMinMax, MinComponent_HandlesNegativeComponents)
{
	const Vec3 v(-1.f, -5.f, -3.f);
	EXPECT_FLOAT_EQ(v.MinComponent(), -5.f);
}

TEST(Vec3_ComponentMinMax, MaxComponent_IgnoresWLane)
{
	// _mm_set_ps(w, z, y, x) — xyz = (1,2,3), w = 100 is the largest lane but must not win.
	const Vec3 v(_mm_set_ps(100.f, 3.f, 2.f, 1.f));
	EXPECT_FLOAT_EQ(v.MaxComponent(), 3.f);
}

TEST(Vec3_ComponentMinMax, MinComponent_IgnoresWLane)
{
	// xyz = (1,2,3), w = -100 is the smallest lane but must not win.
	const Vec3 v(_mm_set_ps(-100.f, 3.f, 2.f, 1.f));
	EXPECT_FLOAT_EQ(v.MinComponent(), 1.f);
}
#pragma endregion-- Vec3_ComponentMinMax

// =============================================================================
// IsInRange
// Componentwise [min, max] check across xyz, bounds inclusive. The w lane must
// never influence the result.
// =============================================================================
#pragma region--Vec3_IsInRange
TEST(Vec3_IsInRange, PointStrictlyInside_ReturnsTrue)
{
	const Vec3 min(-1.f, -1.f, -1.f);
	const Vec3 max(1.f, 1.f, 1.f);
	EXPECT_TRUE(Vec3(0.f, 0.f, 0.f).IsInRange(min, max));
}

TEST(Vec3_IsInRange, PointOnMinBoundary_ReturnsTrue)
{
	const Vec3 min(-1.f, -1.f, -1.f);
	const Vec3 max(1.f, 1.f, 1.f);
	EXPECT_TRUE(Vec3(-1.f, -1.f, -1.f).IsInRange(min, max));
}

TEST(Vec3_IsInRange, PointOnMaxBoundary_ReturnsTrue)
{
	const Vec3 min(-1.f, -1.f, -1.f);
	const Vec3 max(1.f, 1.f, 1.f);
	EXPECT_TRUE(Vec3(1.f, 1.f, 1.f).IsInRange(min, max));
}

TEST(Vec3_IsInRange, PointBelowMinOnOneAxis_ReturnsFalse)
{
	const Vec3 min(-1.f, -1.f, -1.f);
	const Vec3 max(1.f, 1.f, 1.f);
	EXPECT_FALSE(Vec3(0.f, -1.1f, 0.f).IsInRange(min, max));
}

TEST(Vec3_IsInRange, PointAboveMaxOnOneAxis_ReturnsFalse)
{
	const Vec3 min(-1.f, -1.f, -1.f);
	const Vec3 max(1.f, 1.f, 1.f);
	EXPECT_FALSE(Vec3(0.f, 0.f, 1.1f).IsInRange(min, max));
}

TEST(Vec3_IsInRange, WLaneIgnored_DoesNotAffectResult)
{
	const Vec3 min(-1.f, -1.f, -1.f);
	const Vec3 max(1.f, 1.f, 1.f);
	// xyz = (0,0,0) is inside [min, max]; w = 100 is way outside but must be ignored.
	const Vec3 point(_mm_set_ps(100.f, 0.f, 0.f, 0.f));
	EXPECT_TRUE(point.IsInRange(min, max));
}
#pragma endregion-- Vec3_IsInRange

// =============================================================================
// Cross / Dot
// =============================================================================
#pragma region--Vec3_CrossDot
TEST(Vec3_CrossDot, Cross_UnitAxes_FollowsRightHandRule)
{
	const Vec3 x(1.f, 0.f, 0.f);
	const Vec3 y(0.f, 1.f, 0.f);
	EXPECT_EQ(static_cast<glm::vec3>(x.Cross(y)), glm::vec3(0.f, 0.f, 1.f));
}

TEST(Vec3_CrossDot, Cross_GeneralCase_MatchesGlm)
{
	const glm::vec3 ga(1.f, 2.f, 3.f);
	const glm::vec3 gb(4.f, 5.f, 6.f);
	const Vec3		a(ga);
	const Vec3		b(gb);
	EXPECT_EQ(static_cast<glm::vec3>(a.Cross(b)), glm::cross(ga, gb));
}

TEST(Vec3_CrossDot, Dot_GeneralCase_MatchesGlm)
{
	const glm::vec3 ga(1.f, 2.f, 3.f);
	const glm::vec3 gb(4.f, 5.f, 6.f);
	const Vec3		a(ga);
	const Vec3		b(gb);
	EXPECT_FLOAT_EQ(a.Dot(b), glm::dot(ga, gb));
}

TEST(Vec3_CrossDot, Dot_OrthogonalVectors_IsZero)
{
	const Vec3 x(1.f, 0.f, 0.f);
	const Vec3 y(0.f, 1.f, 0.f);
	EXPECT_FLOAT_EQ(x.Dot(y), 0.f);
}
#pragma endregion-- Vec3_CrossDot

// =============================================================================
// distance2
// Squared distance between two points; ignores the w lane, same as Dot.
// =============================================================================
#pragma region--Vec3_Distance2
TEST(Vec3_Distance2, ReturnsSquaredDistanceBetweenPoints)
{
	const Vec3 a(0.f, 0.f, 0.f);
	const Vec3 b(3.f, 4.f, 0.f);
	EXPECT_FLOAT_EQ(distance2(a, b), 25.f);
}

TEST(Vec3_Distance2, IsZeroForIdenticalPoints)
{
	const Vec3 a(1.f, 2.f, 3.f);
	EXPECT_FLOAT_EQ(distance2(a, a), 0.f);
}

TEST(Vec3_Distance2, GeneralCase_MatchesHandComputedValue)
{
	const Vec3 a(1.f, 2.f, 3.f);
	const Vec3 b(4.f, 0.f, -2.f);
	// (1-4)^2 + (2-0)^2 + (3-(-2))^2 = 9 + 4 + 25 = 38
	EXPECT_FLOAT_EQ(distance2(a, b), 38.f);
}

TEST(Vec3_Distance2, IgnoresWLane)
{
	// xyz: (0,0,0) vs (3,4,0) → 25; w differs wildly but must not affect the result.
	const Vec3 a(_mm_set_ps(1000.f, 0.f, 0.f, 0.f));
	const Vec3 b(_mm_set_ps(-1000.f, 0.f, 4.f, 3.f));
	EXPECT_FLOAT_EQ(distance2(a, b), 25.f);
}
#pragma endregion-- Vec3_Distance2

} // namespace Utils::SSE
