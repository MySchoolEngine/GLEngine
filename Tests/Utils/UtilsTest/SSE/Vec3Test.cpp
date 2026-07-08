#include <UtilsTestStdafx.h>

#include <Utils/SSE/Vec3.h>

#include <xmmintrin.h>

namespace Utils::SSE {

namespace {
glm::vec3 ToGlm(const Vec3& v)
{
	alignas(16) float f[4];
	_mm_store_ps(f, v.GetRaw());
	return {f[0], f[1], f[2]};
}
} // namespace

// =============================================================================
// Construction
// =============================================================================
#pragma region--Vec3_Construction
TEST(Vec3_Construction, DefaultConstructor_IsZero)
{
	const Vec3 v;
	EXPECT_EQ(ToGlm(v), glm::vec3(0.f));
}

TEST(Vec3_Construction, ScalarConstructor_BroadcastsToXYZ)
{
	const Vec3 v(2.5f);
	EXPECT_EQ(ToGlm(v), glm::vec3(2.5f));
}

TEST(Vec3_Construction, GlmVec3Constructor_CopiesComponents)
{
	const Vec3 v(glm::vec3(1.f, 2.f, 3.f));
	EXPECT_EQ(ToGlm(v), glm::vec3(1.f, 2.f, 3.f));
}

TEST(Vec3_Construction, XYZConstructor_SetsComponents)
{
	const Vec3 v(1.f, 2.f, 3.f);
	EXPECT_EQ(ToGlm(v), glm::vec3(1.f, 2.f, 3.f));
}
#pragma endregion-- Vec3_Construction

// =============================================================================
// Arithmetic operators
// =============================================================================
#pragma region--Vec3_Arithmetic
TEST(Vec3_Arithmetic, Subtraction_SubtractsComponentwise)
{
	const Vec3 a(5.f, 3.f, 1.f);
	const Vec3 b(1.f, 2.f, 3.f);
	EXPECT_EQ(ToGlm(a - b), glm::vec3(4.f, 1.f, -2.f));
}

TEST(Vec3_Arithmetic, Addition_AddsComponentwise)
{
	const Vec3 a(1.f, 2.f, 3.f);
	const Vec3 b(4.f, 5.f, 6.f);
	EXPECT_EQ(ToGlm(a + b), glm::vec3(5.f, 7.f, 9.f));
}

TEST(Vec3_Arithmetic, Multiplication_MultipliesComponentwise)
{
	const Vec3 a(2.f, 3.f, 4.f);
	const Vec3 b(5.f, 6.f, 7.f);
	EXPECT_EQ(ToGlm(a * b), glm::vec3(10.f, 18.f, 28.f));
}

TEST(Vec3_Arithmetic, Division_DividesComponentwise)
{
	const Vec3 a(10.f, 18.f, 28.f);
	const Vec3 b(2.f, 3.f, 4.f);
	EXPECT_EQ(ToGlm(a / b), glm::vec3(5.f, 6.f, 7.f));
}

TEST(Vec3_Arithmetic, ScalarMultiplication_ScalesEachComponent)
{
	const Vec3 a(1.f, 2.f, 3.f);
	EXPECT_EQ(ToGlm(a * 2.f), glm::vec3(2.f, 4.f, 6.f));
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
	EXPECT_EQ(ToGlm(Vec3::min(a, b)), glm::vec3(1.f, 2.f, -4.f));
}

TEST(Vec3_MinMax, Max_TakesComponentwiseMaximum)
{
	const Vec3 a(1.f, 5.f, -2.f);
	const Vec3 b(3.f, 2.f, -4.f);
	EXPECT_EQ(ToGlm(Vec3::max(a, b)), glm::vec3(3.f, 5.f, -2.f));
}
#pragma endregion-- Vec3_MinMax

// =============================================================================
// Cross / Dot
// =============================================================================
#pragma region--Vec3_CrossDot
TEST(Vec3_CrossDot, Cross_UnitAxes_FollowsRightHandRule)
{
	const Vec3 x(1.f, 0.f, 0.f);
	const Vec3 y(0.f, 1.f, 0.f);
	EXPECT_EQ(ToGlm(x.Cross(y)), glm::vec3(0.f, 0.f, 1.f));
}

TEST(Vec3_CrossDot, Cross_GeneralCase_MatchesGlm)
{
	const glm::vec3 ga(1.f, 2.f, 3.f);
	const glm::vec3 gb(4.f, 5.f, 6.f);
	const Vec3		a(ga);
	const Vec3		b(gb);
	EXPECT_EQ(ToGlm(a.Cross(b)), glm::cross(ga, gb));
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

} // namespace Utils::SSE
