#include <PhysicsTestStdafx.h>

#include <Physics/Primitives/AABB.h>
#include <Physics/Primitives/Ray.h>
#include <Physics/Primitives/Sphere.h>

#include <glm/gtc/matrix_transform.hpp>
#include <limits>

namespace GLEngine::Physics::Primitives {

// =============================================================================
// Initialization
// =============================================================================
using AABBTypes = ::testing::Types<S_AABB, S_SSEAABB>;
template<class T>
class AABB_Base {
public:
	using VecType = decltype(std::declval<T>().m_Min);
};
template<class T>
class AABB_Initialization : public ::testing::Test, public AABB_Base<T> {};
template<class T>
class AABB_AddPoint : public ::testing::Test, public AABB_Base<T> {};
template<class T>
class AABB_AddAABB : public ::testing::Test {};
template<class T>
class AABB_Area : public ::testing::Test {};
template<class T>
class AABB_ContainsPoint : public ::testing::Test, public AABB_Base<T> {};
template<class T_AABB, class T_Ray>
struct IntersectParams {
	using AABBType = T_AABB;
	using RayType  = T_Ray;
};

using IntersectTypes = ::testing::Types<
	IntersectParams<S_AABB, S_Ray>,
	IntersectParams<S_AABB, S_SSERay>,
	IntersectParams<S_SSEAABB, S_SSERay>
>;

template<class Params>
class AABB_Intersects : public ::testing::Test {
public:
	using AABBType = typename Params::AABBType;
	using RayType  = typename Params::RayType;
	using VecType  = decltype(std::declval<AABBType>().m_Min);
};

TYPED_TEST_SUITE(AABB_Initialization, AABBTypes);
TYPED_TEST_SUITE(AABB_AddPoint, AABBTypes);
TYPED_TEST_SUITE(AABB_AddAABB, AABBTypes);
TYPED_TEST_SUITE(AABB_Area, AABBTypes);
TYPED_TEST_SUITE(AABB_ContainsPoint, AABBTypes);
TYPED_TEST_SUITE(AABB_Intersects, IntersectTypes);

#pragma region --AABB_Initialization
TYPED_TEST(AABB_Initialization, DefaultConstructed_NotInitialized)
{
	TypeParam aabb;
	EXPECT_FALSE(aabb.IsInitialized());
}

TYPED_TEST(AABB_Initialization, AfterAddPoint_IsInitialized)
{
	TypeParam aabb;
	aabb.Add({1.f, 2.f, 3.f});
	EXPECT_TRUE(aabb.IsInitialized());
}
#pragma endregion --AABB_Initialization

// =============================================================================
// Add(vec3)
// =============================================================================
#pragma region --AABB_AddPoint
TYPED_TEST(AABB_AddPoint, AddSinglePoint_MinEqualsMax)
{
	TypeParam aabb;
	const typename TestFixture::VecType p{1.f, 2.f, 3.f};
	aabb.Add(p);
	EXPECT_EQ(aabb.m_Min, p);
	EXPECT_EQ(aabb.m_Max, p);
}

TYPED_TEST(AABB_AddPoint, AddMultiplePoints_ExpandsMinAndMax)
{
	TypeParam aabb;
	aabb.Add(typename TestFixture::VecType( 0.f, 0.f, 0.f ));
	aabb.Add(typename TestFixture::VecType( 2.f, 3.f, 4.f ));
	aabb.Add(typename TestFixture::VecType( -1.f, 5.f, 2.f ));

	EXPECT_EQ(aabb.m_Min, typename TestFixture::VecType(-1.f, 0.f, 0.f));
	EXPECT_EQ(aabb.m_Max, typename TestFixture::VecType(2.f, 5.f, 4.f));
}

TEST(AABB_AddPoint, AddVec4Point_StripsWComponent)
{
	S_AABB aabb;
	aabb.Add(glm::vec4(1.f, 2.f, 3.f, 99.f)); // w must be ignored
	EXPECT_EQ(aabb.m_Min, glm::vec3(1.f, 2.f, 3.f));
	EXPECT_EQ(aabb.m_Max, glm::vec3(1.f, 2.f, 3.f));
}
#pragma endregion --AABB_AddPoint

// =============================================================================
// Add(AABB)
// =============================================================================
#pragma region --AABB_AddAABB
TYPED_TEST(AABB_AddAABB, AddUninitializedAABB_IsNoOp)
{
	TypeParam aabb;
	aabb.Add(decltype(aabb.m_Min)(0.f));
	aabb.Add(decltype(aabb.m_Min)(1.f));

	const TypeParam empty;
	aabb.Add(empty);

	EXPECT_EQ(aabb.m_Min, decltype(aabb.m_Min)(0.f));
	EXPECT_EQ(aabb.m_Max, decltype(aabb.m_Min)(1.f));
}

TYPED_TEST(AABB_AddAABB, AddInitializedAABB_MergesCorrectly)
{
	using VecType = decltype(TypeParam::m_Min);
	TypeParam a;
	a.Add(VecType(0.f));
	a.Add(VecType(1.f));

	TypeParam b;
	b.Add(VecType(-1.f));
	b.Add(VecType(2.f));

	a.Add(b);

	EXPECT_EQ(a.m_Min, VecType(-1.f));
	EXPECT_EQ(a.m_Max, VecType(2.f));
}
#pragma endregion --AABB_AddAABB

// =============================================================================
// Contains(point)
// The box limits are part of the box (inclusive >= and <=).
// Tests document and lock in this contract before any refactor.
// =============================================================================
#pragma region --AABB_ContainsPoint
TYPED_TEST(AABB_ContainsPoint, ContainsPoint_StrictlyInside)
{
	TypeParam aabb;
	aabb.Add(typename TestFixture::VecType(-1.f));
	aabb.Add(typename TestFixture::VecType(1.f));
	EXPECT_TRUE(aabb.Contains(typename TestFixture::VecType(0.f, 0.f, 0.f)));
}

TYPED_TEST(AABB_ContainsPoint, ContainsPoint_OnMinXFace)
{
	TypeParam aabb;
	aabb.Add(typename TestFixture::VecType(-1.f));
	aabb.Add(typename TestFixture::VecType(1.f));
	// Boundary is inclusive (point.x >= m_Min.x)
	EXPECT_TRUE(aabb.Contains(typename TestFixture::VecType(-1.f, 0.f, 0.f)));
}

TYPED_TEST(AABB_ContainsPoint, ContainsPoint_OnMaxXFace)
{
	TypeParam aabb;
	aabb.Add(typename TestFixture::VecType(-1.f));
	aabb.Add(typename TestFixture::VecType(1.f));
	// Boundary is inclusive (point.x <= m_Max.x)
	EXPECT_TRUE(aabb.Contains(typename TestFixture::VecType(1.f, 0.f, 0.f)));
}

TYPED_TEST(AABB_ContainsPoint, ContainsPoint_OnMinYFace)
{
	TypeParam aabb;
	aabb.Add(typename TestFixture::VecType(-1.f));
	aabb.Add(typename TestFixture::VecType(1.f));
	EXPECT_TRUE(aabb.Contains(typename TestFixture::VecType(0.f, -1.f, 0.f)));
}

TYPED_TEST(AABB_ContainsPoint, ContainsPoint_OnMaxYFace)
{
	TypeParam aabb;
	aabb.Add(typename TestFixture::VecType(-1.f));
	aabb.Add(typename TestFixture::VecType(1.f));
	EXPECT_TRUE(aabb.Contains(typename TestFixture::VecType(0.f, 1.f, 0.f)));
}

TYPED_TEST(AABB_ContainsPoint, ContainsPoint_OnMinZFace)
{
	TypeParam aabb;
	aabb.Add(typename TestFixture::VecType(-1.f));
	aabb.Add(typename TestFixture::VecType(1.f));
	EXPECT_TRUE(aabb.Contains(typename TestFixture::VecType(0.f, 0.f, -1.f)));
}

TYPED_TEST(AABB_ContainsPoint, ContainsPoint_OnMaxZFace)
{
	TypeParam aabb;
	aabb.Add(typename TestFixture::VecType(-1.f));
	aabb.Add(typename TestFixture::VecType(1.f));
	EXPECT_TRUE(aabb.Contains(typename TestFixture::VecType(0.f, 0.f, 1.f)));
}

TYPED_TEST(AABB_ContainsPoint, ContainsPoint_OnCorner)
{
	TypeParam aabb;
	aabb.Add(typename TestFixture::VecType(-1.f));
	aabb.Add(typename TestFixture::VecType(1.f));
	// All three axes at their limit simultaneously — still inside
	EXPECT_TRUE(aabb.Contains(typename TestFixture::VecType(1.f, 1.f, 1.f)));
	EXPECT_TRUE(aabb.Contains(typename TestFixture::VecType(-1.f, -1.f, -1.f)));
}

TYPED_TEST(AABB_ContainsPoint, ContainsPoint_OutsideBox)
{
	TypeParam aabb;
	aabb.Add(typename TestFixture::VecType(-1.f));
	aabb.Add(typename TestFixture::VecType(1.f));
	EXPECT_FALSE(aabb.Contains(typename TestFixture::VecType(2.f, 0.f, 0.f)));
	EXPECT_FALSE(aabb.Contains(typename TestFixture::VecType(0.f, -2.f, 0.f)));
	EXPECT_FALSE(aabb.Contains(typename TestFixture::VecType(0.f, 0.f, 1.1f)));
}

TYPED_TEST(AABB_ContainsPoint, ContainsPoint_UninitializedBox)
{
	const TypeParam aabb;
	EXPECT_FALSE(aabb.Contains(typename TestFixture::VecType(0.f, 0.f, 0.f)));
}
#pragma endregion --AABB_ContainsPoint

// =============================================================================
// Contains(AABB)
// Delegates to Contains(point) for min and max corners, so boundary rules are
// the same: touching the outer limit still counts as contained.
// =============================================================================
#pragma region --AABB_ContainsAABB
TEST(AABB_ContainsAABB, ContainsAABB_FullyInside)
{
	S_AABB outer;
	outer.Add(glm::vec3(-2.f));
	outer.Add(glm::vec3(2.f));

	S_AABB inner;
	inner.Add(glm::vec3(-1.f));
	inner.Add(glm::vec3(1.f));

	EXPECT_TRUE(outer.Contains(inner));
}

TEST(AABB_ContainsAABB, ContainsAABB_SameBox)
{
	S_AABB aabb;
	aabb.Add(glm::vec3(-1.f));
	aabb.Add(glm::vec3(1.f));
	// A box contains itself (limits are inclusive)
	EXPECT_TRUE(aabb.Contains(aabb));
}

TEST(AABB_ContainsAABB, ContainsAABB_BoundariesTouching)
{
	S_AABB outer;
	outer.Add(glm::vec3(-2.f));
	outer.Add(glm::vec3(2.f));

	// inner shares one face exactly with outer (inner.m_Min.x == outer.m_Min.x)
	S_AABB inner;
	inner.Add(glm::vec3(-2.f, -1.f, -1.f));
	inner.Add(glm::vec3(1.f, 1.f, 1.f));

	// inner.m_Min.x is exactly at the outer limit — boundary is inclusive
	EXPECT_TRUE(outer.Contains(inner));
}

TEST(AABB_ContainsAABB, ContainsAABB_PartialOverlap)
{
	S_AABB outer;
	outer.Add(glm::vec3(-1.f));
	outer.Add(glm::vec3(1.f));

	S_AABB overlapping;
	overlapping.Add(glm::vec3(0.f));
	overlapping.Add(glm::vec3(3.f)); // extends beyond outer

	EXPECT_FALSE(outer.Contains(overlapping));
}

TEST(AABB_ContainsAABB, ContainsAABB_OuterUninitialized)
{
	const S_AABB outer;

	S_AABB inner;
	inner.Add(glm::vec3(-1.f));
	inner.Add(glm::vec3(1.f));

	EXPECT_FALSE(outer.Contains(inner));
}

TEST(AABB_ContainsAABB, ContainsAABB_InnerUninitialized)
{
	S_AABB outer;
	outer.Add(glm::vec3(-1.f));
	outer.Add(glm::vec3(1.f));

	const S_AABB inner;
	EXPECT_FALSE(outer.Contains(inner));
}
#pragma endregion --AABB_ContainsAABB

// =============================================================================
// Area
// =============================================================================
#pragma region --AABB_Area
TYPED_TEST(AABB_Area, Area_UnitCube)
{
	using VecType = decltype(TypeParam::m_Min);
	TypeParam aabb;
	aabb.Add(VecType(0.f));
	aabb.Add(VecType(1.f));
	// 6 unit faces
	EXPECT_FLOAT_EQ(aabb.Area(), 6.0f);
}

TYPED_TEST(AABB_Area, Area_DegeneratePoint)
{
	using VecType = decltype(TypeParam::m_Min);
	TypeParam aabb;
	aabb.Add(VecType(5.f, 3.f, 1.f));
	// All extents are 0
	EXPECT_FLOAT_EQ(aabb.Area(), 0.0f);
}

TYPED_TEST(AABB_Area, Area_NonUniformBox)
{
	using VecType = decltype(TypeParam::m_Min);
	TypeParam aabb;
	aabb.Add(VecType(0.f));
	aabb.Add(VecType(2.f, 3.f, 4.f));
	// (2*3 + 3*4 + 4*2) * 2 = (6 + 12 + 8) * 2 = 52
	EXPECT_FLOAT_EQ(aabb.Area(), 52.0f);
}
#pragma endregion --AABB_Area

// =============================================================================
// GetSphere
// =============================================================================
#pragma region --AABB_GetSphere
TEST(AABB_GetSphere, GetSphere_CenterIsMidpoint)
{
	S_AABB aabb;
	aabb.Add(glm::vec3(0.f));
	aabb.Add(glm::vec3(2.f));

	const auto sphere = aabb.GetSphere();
	EXPECT_EQ(sphere.m_position, glm::vec3(1.f));
}

TEST(AABB_GetSphere, GetSphere_RadiusIsDistanceToCentreToMax)
{
	S_AABB aabb;
	aabb.Add(glm::vec3(0.f));
	aabb.Add(glm::vec3(2.f));

	const auto sphere = aabb.GetSphere();
	// center=(1,1,1), max=(2,2,2): distance = sqrt((1)^2+(1)^2+(1)^2) = sqrt(3)
	EXPECT_FLOAT_EQ(sphere.m_radius, std::sqrt(3.0f));
}
#pragma endregion --AABB_GetSphere

// =============================================================================
// getTransformedAABB
// =============================================================================
#pragma region --AABB_Transform
TEST(AABB_Transform, TransformedAABB_Uninitialized)
{
	const S_AABB aabb;
	const auto result = aabb.getTransformedAABB(glm::mat4(1.0f));
	EXPECT_FALSE(result.IsInitialized());
}

TEST(AABB_Transform, TransformedAABB_Identity)
{
	S_AABB aabb;
	aabb.Add(glm::vec3(-1.f));
	aabb.Add(glm::vec3(1.f));

	const auto result = aabb.getTransformedAABB(glm::mat4(1.0f));

	EXPECT_TRUE(result.IsInitialized());
	EXPECT_NEAR(result.m_Min.x, -1.f, 1e-5f);
	EXPECT_NEAR(result.m_Min.y, -1.f, 1e-5f);
	EXPECT_NEAR(result.m_Min.z, -1.f, 1e-5f);
	EXPECT_NEAR(result.m_Max.x, 1.f, 1e-5f);
	EXPECT_NEAR(result.m_Max.y, 1.f, 1e-5f);
	EXPECT_NEAR(result.m_Max.z, 1.f, 1e-5f);
}

TEST(AABB_Transform, TransformedAABB_Translation)
{
	S_AABB aabb;
	aabb.Add(glm::vec3(0.f));
	aabb.Add(glm::vec3(1.f));

	const glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(5.f, 0.f, 0.f));
	const auto result = aabb.getTransformedAABB(translate);

	EXPECT_NEAR(result.m_Min.x, 5.f, 1e-5f);
	EXPECT_NEAR(result.m_Max.x, 6.f, 1e-5f);
}
#pragma endregion --AABB_Transform

// =============================================================================
// Intersects — primary slab algorithm
// Returns the entry t-parameter (0 when origin is inside, infinity on miss).
// =============================================================================
#pragma region --AABB_Intersects
TYPED_TEST(AABB_Intersects, Intersects_RayHits)
{
	using AABBType = typename TestFixture::AABBType;
	using RayType  = typename TestFixture::RayType;
	using VecType  = typename TestFixture::VecType;

	AABBType aabb;
	aabb.Add(VecType(1.f, -1.f, -1.f));
	aabb.Add(VecType(3.f,  1.f,  1.f));

	// Axis-aligned ray along +x; enters box face at x=1
	const RayType ray{{0.f, 0.f, 0.f}, {1.f, 0.f, 0.f}};
	const float t = aabb.Intersects(ray);

	EXPECT_NE(t, std::numeric_limits<float>::infinity());
	EXPECT_FLOAT_EQ(t, 1.0f);
}

TYPED_TEST(AABB_Intersects, Intersects_RayMisses)
{
	using AABBType = typename TestFixture::AABBType;
	using RayType  = typename TestFixture::RayType;
	using VecType  = typename TestFixture::VecType;

	AABBType aabb;
	aabb.Add(VecType(1.f, 1.f, 1.f));
	aabb.Add(VecType(3.f, 3.f, 3.f));

	// Ray travels along +x at y=0, z=0 — misses box which starts at y=1, z=1
	const RayType ray{{0.f, 0.f, 0.f}, {1.f, 0.f, 0.f}};
	EXPECT_EQ(aabb.Intersects(ray), std::numeric_limits<float>::infinity());
}

TYPED_TEST(AABB_Intersects, Intersects_RayFromInside)
{
	using AABBType = typename TestFixture::AABBType;
	using RayType  = typename TestFixture::RayType;
	using VecType  = typename TestFixture::VecType;

	AABBType aabb;
	aabb.Add(VecType(-1.f, -1.f, -1.f));
	aabb.Add(VecType( 1.f,  1.f,  1.f));

	// Ray originates inside box — entry t is 0
	const RayType ray{{0.f, 0.f, 0.f}, {1.f, 0.f, 0.f}};
	EXPECT_FLOAT_EQ(aabb.Intersects(ray), 0.0f);
}

TYPED_TEST(AABB_Intersects, Intersects_RayParallelToAxis_OriginOnBoundary)
{
	using AABBType = typename TestFixture::AABBType;
	using RayType  = typename TestFixture::RayType;
	using VecType  = typename TestFixture::VecType;

	AABBType aabb;
	aabb.Add(VecType(-1.f, -1.f, -1.f));
	aabb.Add(VecType( 1.f,  1.f,  1.f));

	// Ray travels along +x with its origin sitting exactly on the y = m_Min
	// boundary. direction.y == 0 makes invDirection.y == +inf, and
	// (m_Min.y - origin.y) == 0, so the slab math produces a 0 * inf == NaN
	// intermediate that must be corrected before the min/max reduction.
	const RayType ray{{0.f, -1.f, 0.f}, {1.f, 0.f, 0.f}};

	// Origin lies on the box surface, so the ray is already "inside" — entry
	// t is 0, not a miss.
	EXPECT_FLOAT_EQ(aabb.Intersects(ray), 0.0f);
}

TYPED_TEST(AABB_Intersects, Intersects_UninitializedBox)
{
	using AABBType = typename TestFixture::AABBType;
	using RayType  = typename TestFixture::RayType;

	const AABBType aabb;
	const RayType ray{{0.f, 0.f, 0.f}, {1.f, 0.f, 0.f}};
	EXPECT_EQ(aabb.Intersects(ray), std::numeric_limits<float>::infinity());
}

TYPED_TEST(AABB_Intersects, Intersects_UninitializedBox_NegativeDirection)
{
	using AABBType = typename TestFixture::AABBType;
	using RayType  = typename TestFixture::RayType;

	const AABBType aabb;
	const RayType ray{{5.f, 5.f, 5.f}, {-1.f, 0.f, 0.f}};
	EXPECT_EQ(aabb.Intersects(ray), std::numeric_limits<float>::infinity());
}

TYPED_TEST(AABB_Intersects, Intersects_UninitializedBox_DiagonalDirection)
{
	using AABBType = typename TestFixture::AABBType;
	using RayType  = typename TestFixture::RayType;

	const AABBType aabb;
	const glm::vec3 dir = glm::normalize(glm::vec3(1.f, 1.f, 1.f));
	const RayType ray{{0.f, 0.f, 0.f}, dir};
	EXPECT_EQ(aabb.Intersects(ray), std::numeric_limits<float>::infinity());
}

TYPED_TEST(AABB_Intersects, Intersects_UninitializedBox_AxisAlignedAxes)
{
	using AABBType = typename TestFixture::AABBType;
	using RayType  = typename TestFixture::RayType;

	// Ensure the guard fires for all three principal axes
	const AABBType aabb;
	EXPECT_EQ(aabb.Intersects(RayType{{0.f, 0.f, 0.f}, {0.f, 1.f, 0.f}}), std::numeric_limits<float>::infinity());
	EXPECT_EQ(aabb.Intersects(RayType{{0.f, 0.f, 0.f}, {0.f, 0.f, 1.f}}), std::numeric_limits<float>::infinity());
}
#pragma endregion --AABB_Intersects

// =============================================================================
// IntersectImpl — Graphics Gems ray-box algorithm
// Returns distance to hit point (>0), 0.0 when inside, negative on miss.
// =============================================================================
#pragma region --AABB_IntersectImpl
TEST(AABB_IntersectImpl, IntersectImpl_RayHits)
{
	S_AABB aabb;
	aabb.Add(glm::vec3(1.f, -1.f, -1.f));
	aabb.Add(glm::vec3(3.f, 1.f, 1.f));

	// Axis-aligned ray; hits the x=1 face at distance 1
	const S_Ray ray{{0.f, 0.f, 0.f}, {1.f, 0.f, 0.f}};
	const float dist = aabb.IntersectImpl(ray);

	EXPECT_FLOAT_EQ(dist, 1.0f);
}

TEST(AABB_IntersectImpl, IntersectImpl_RayMisses)
{
	S_AABB aabb;
	aabb.Add(glm::vec3(1.f, 1.f, 1.f));
	aabb.Add(glm::vec3(3.f, 3.f, 3.f));

	// Ray at y=0, z=0 — cannot reach box that starts at y=1, z=1
	const S_Ray ray{{0.f, 0.f, 0.f}, {1.f, 0.f, 0.f}};
	EXPECT_EQ(aabb.IntersectImpl(ray), std::numeric_limits<float>::infinity());
}

TEST(AABB_IntersectImpl, IntersectImpl_RayFromInside)
{
	S_AABB aabb;
	aabb.Add(glm::vec3(-1.f));
	aabb.Add(glm::vec3(1.f));

	// Origin inside box — Graphics Gems algorithm returns 0
	const S_Ray ray{{0.f, 0.f, 0.f}, {1.f, 0.f, 0.f}};
	EXPECT_FLOAT_EQ(aabb.IntersectImpl(ray), 0.0f);
}

TEST(AABB_IntersectImpl, IntersectImpl_Uninitialized)
{
	const S_AABB aabb;
	const S_Ray ray{{0.f, 0.f, 0.f}, {1.f, 0.f, 0.f}};
	EXPECT_EQ(aabb.IntersectImpl(ray), std::numeric_limits<float>::infinity());
}

TEST(AABB_IntersectImpl, IntersectImpl_Uninitialized_NegativeDirection)
{
	const S_AABB aabb;
	const S_Ray ray{{5.f, 5.f, 5.f}, {-1.f, 0.f, 0.f}};
	EXPECT_EQ(aabb.IntersectImpl(ray), std::numeric_limits<float>::infinity());
}

TEST(AABB_IntersectImpl, IntersectImpl_Uninitialized_DiagonalDirection)
{
	const S_AABB aabb;
	const glm::vec3 dir = glm::normalize(glm::vec3(1.f, 1.f, 1.f));
	const S_Ray ray{{0.f, 0.f, 0.f}, dir};
	EXPECT_EQ(aabb.IntersectImpl(ray), std::numeric_limits<float>::infinity());
}

TEST(AABB_IntersectImpl, IntersectImpl_Uninitialized_AxisAlignedAxes)
{
	const S_AABB aabb;
	EXPECT_EQ(aabb.IntersectImpl(S_Ray{{0.f, 0.f, 0.f}, {0.f, 1.f, 0.f}}), std::numeric_limits<float>::infinity());
	EXPECT_EQ(aabb.IntersectImpl(S_Ray{{0.f, 0.f, 0.f}, {0.f, 0.f, 1.f}}), std::numeric_limits<float>::infinity());
}
#pragma endregion --AABB_IntersectImpl

// =============================================================================
// Add(sphere)
// Documents the current behavior: the implementation ignores sphere.m_radius
// and always adds six axis-aligned points at ±1 from the sphere center.
// This is a known refactor target — these tests lock in the current contract.
// =============================================================================
#pragma region --AABB_AddSphere
TEST(AABB_AddSphere, AddSphere_HardcodedUnitRadius)
{
	S_AABB aabb;
	// Sphere at origin with a large radius — but the current Add(sphere) impl
	// hardcodes ±1 offsets, so m_radius is effectively ignored.
	const S_Sphere sphere{{0.f, 0.f, 0.f}, 10.f};
	aabb.Add(sphere);

	EXPECT_EQ(aabb.m_Min, glm::vec3(-10.f));
	EXPECT_EQ(aabb.m_Max, glm::vec3(10.f));
}
#pragma endregion --AABB_AddSphere

// =============================================================================
// updateWithTriangle
// =============================================================================
#pragma region --AABB_UpdateWithTriangle
TEST(AABB_UpdateWithTriangle, UpdateWithTriangle_EncompassesAllVertices)
{
	S_AABB aabb;
	const glm::vec3 vertices[3] = {
		{0.f, 0.f, 0.f},
		{3.f, 0.f, 0.f},
		{0.f, 4.f, 0.f},
	};
	aabb.updateWithTriangle(vertices);

	EXPECT_TRUE(aabb.IsInitialized());
	EXPECT_EQ(aabb.m_Min, glm::vec3(0.f, 0.f, 0.f));
	EXPECT_EQ(aabb.m_Max, glm::vec3(3.f, 4.f, 0.f));
	EXPECT_TRUE(aabb.Contains(glm::vec3(0.f, 0.f, 0.f)));
	EXPECT_TRUE(aabb.Contains(glm::vec3(3.f, 0.f, 0.f)));
	EXPECT_TRUE(aabb.Contains(glm::vec3(0.f, 4.f, 0.f)));
}
#pragma endregion --AABB_UpdateWithTriangle

} // namespace GLEngine::Physics::Primitives
