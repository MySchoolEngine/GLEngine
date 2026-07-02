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

TEST(AABB_Initialization, DefaultConstructed_NotInitialized)
{
	const S_AABB aabb;
	EXPECT_FALSE(aabb.IsInitialized());
}

TEST(AABB_Initialization, AfterAddPoint_IsInitialized)
{
	S_AABB aabb;
	aabb.Add(glm::vec3(1.f, 2.f, 3.f));
	EXPECT_TRUE(aabb.IsInitialized());
}

// =============================================================================
// Add(vec3)
// =============================================================================

TEST(AABB_AddPoint, AddSinglePoint_MinEqualsMax)
{
	S_AABB aabb;
	const glm::vec3 p{1.f, 2.f, 3.f};
	aabb.Add(p);
	EXPECT_EQ(aabb.m_Min, p);
	EXPECT_EQ(aabb.m_Max, p);
}

TEST(AABB_AddPoint, AddMultiplePoints_ExpandsMinAndMax)
{
	S_AABB aabb;
	aabb.Add(glm::vec3(0.f, 0.f, 0.f));
	aabb.Add(glm::vec3(2.f, 3.f, 4.f));
	aabb.Add(glm::vec3(-1.f, 5.f, 2.f));

	EXPECT_EQ(aabb.m_Min, glm::vec3(-1.f, 0.f, 0.f));
	EXPECT_EQ(aabb.m_Max, glm::vec3(2.f, 5.f, 4.f));
}

TEST(AABB_AddPoint, AddVec4Point_StripsWComponent)
{
	S_AABB aabb;
	aabb.Add(glm::vec4(1.f, 2.f, 3.f, 99.f)); // w must be ignored
	EXPECT_EQ(aabb.m_Min, glm::vec3(1.f, 2.f, 3.f));
	EXPECT_EQ(aabb.m_Max, glm::vec3(1.f, 2.f, 3.f));
}

// =============================================================================
// Add(AABB)
// =============================================================================

TEST(AABB_AddAABB, AddUninitializedAABB_IsNoOp)
{
	S_AABB aabb;
	aabb.Add(glm::vec3(0.f));
	aabb.Add(glm::vec3(1.f));

	const S_AABB empty;
	aabb.Add(empty);

	EXPECT_EQ(aabb.m_Min, glm::vec3(0.f));
	EXPECT_EQ(aabb.m_Max, glm::vec3(1.f));
}

TEST(AABB_AddAABB, AddInitializedAABB_MergesCorrectly)
{
	S_AABB a;
	a.Add(glm::vec3(0.f));
	a.Add(glm::vec3(1.f));

	S_AABB b;
	b.Add(glm::vec3(-1.f));
	b.Add(glm::vec3(2.f));

	a.Add(b);

	EXPECT_EQ(a.m_Min, glm::vec3(-1.f));
	EXPECT_EQ(a.m_Max, glm::vec3(2.f));
}

// =============================================================================
// Contains(point)
// The box limits are part of the box (inclusive >= and <=).
// Tests document and lock in this contract before any refactor.
// =============================================================================

TEST(AABB_ContainsPoint, ContainsPoint_StrictlyInside)
{
	S_AABB aabb;
	aabb.Add(glm::vec3(-1.f));
	aabb.Add(glm::vec3(1.f));
	EXPECT_TRUE(aabb.Contains(glm::vec3(0.f, 0.f, 0.f)));
}

TEST(AABB_ContainsPoint, ContainsPoint_OnMinXFace)
{
	S_AABB aabb;
	aabb.Add(glm::vec3(-1.f));
	aabb.Add(glm::vec3(1.f));
	// Boundary is inclusive (point.x >= m_Min.x)
	EXPECT_TRUE(aabb.Contains(glm::vec3(-1.f, 0.f, 0.f)));
}

TEST(AABB_ContainsPoint, ContainsPoint_OnMaxXFace)
{
	S_AABB aabb;
	aabb.Add(glm::vec3(-1.f));
	aabb.Add(glm::vec3(1.f));
	// Boundary is inclusive (point.x <= m_Max.x)
	EXPECT_TRUE(aabb.Contains(glm::vec3(1.f, 0.f, 0.f)));
}

TEST(AABB_ContainsPoint, ContainsPoint_OnMinYFace)
{
	S_AABB aabb;
	aabb.Add(glm::vec3(-1.f));
	aabb.Add(glm::vec3(1.f));
	EXPECT_TRUE(aabb.Contains(glm::vec3(0.f, -1.f, 0.f)));
}

TEST(AABB_ContainsPoint, ContainsPoint_OnMaxYFace)
{
	S_AABB aabb;
	aabb.Add(glm::vec3(-1.f));
	aabb.Add(glm::vec3(1.f));
	EXPECT_TRUE(aabb.Contains(glm::vec3(0.f, 1.f, 0.f)));
}

TEST(AABB_ContainsPoint, ContainsPoint_OnMinZFace)
{
	S_AABB aabb;
	aabb.Add(glm::vec3(-1.f));
	aabb.Add(glm::vec3(1.f));
	EXPECT_TRUE(aabb.Contains(glm::vec3(0.f, 0.f, -1.f)));
}

TEST(AABB_ContainsPoint, ContainsPoint_OnMaxZFace)
{
	S_AABB aabb;
	aabb.Add(glm::vec3(-1.f));
	aabb.Add(glm::vec3(1.f));
	EXPECT_TRUE(aabb.Contains(glm::vec3(0.f, 0.f, 1.f)));
}

TEST(AABB_ContainsPoint, ContainsPoint_OnCorner)
{
	S_AABB aabb;
	aabb.Add(glm::vec3(-1.f));
	aabb.Add(glm::vec3(1.f));
	// All three axes at their limit simultaneously — still inside
	EXPECT_TRUE(aabb.Contains(glm::vec3(1.f, 1.f, 1.f)));
	EXPECT_TRUE(aabb.Contains(glm::vec3(-1.f, -1.f, -1.f)));
}

TEST(AABB_ContainsPoint, ContainsPoint_OutsideBox)
{
	S_AABB aabb;
	aabb.Add(glm::vec3(-1.f));
	aabb.Add(glm::vec3(1.f));
	EXPECT_FALSE(aabb.Contains(glm::vec3(2.f, 0.f, 0.f)));
	EXPECT_FALSE(aabb.Contains(glm::vec3(0.f, -2.f, 0.f)));
	EXPECT_FALSE(aabb.Contains(glm::vec3(0.f, 0.f, 1.1f)));
}

TEST(AABB_ContainsPoint, ContainsPoint_UninitializedBox)
{
	const S_AABB aabb;
	EXPECT_FALSE(aabb.Contains(glm::vec3(0.f, 0.f, 0.f)));
}

// =============================================================================
// Contains(AABB)
// Delegates to Contains(point) for min and max corners, so boundary rules are
// the same: touching the outer limit still counts as contained.
// =============================================================================

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

// =============================================================================
// Area
// =============================================================================

TEST(AABB_Area, Area_UnitCube)
{
	S_AABB aabb;
	aabb.Add(glm::vec3(0.f));
	aabb.Add(glm::vec3(1.f));
	// 6 unit faces
	EXPECT_FLOAT_EQ(aabb.Area(), 6.0f);
}

TEST(AABB_Area, Area_DegeneratePoint)
{
	S_AABB aabb;
	aabb.Add(glm::vec3(5.f, 3.f, 1.f));
	// All extents are 0
	EXPECT_FLOAT_EQ(aabb.Area(), 0.0f);
}

TEST(AABB_Area, Area_NonUniformBox)
{
	S_AABB aabb;
	aabb.Add(glm::vec3(0.f));
	aabb.Add(glm::vec3(2.f, 3.f, 4.f));
	// (2*3 + 3*4 + 4*2) * 2 = (6 + 12 + 8) * 2 = 52
	EXPECT_FLOAT_EQ(aabb.Area(), 52.0f);
}

// =============================================================================
// GetSphere
// =============================================================================

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

// =============================================================================
// getTransformedAABB
// =============================================================================

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

// =============================================================================
// Intersects — primary slab algorithm
// Returns the entry t-parameter (0 when origin is inside, infinity on miss).
// =============================================================================

TEST(AABB_Intersects, Intersects_RayHits)
{
	S_AABB aabb;
	aabb.Add(glm::vec3(1.f, -1.f, -1.f));
	aabb.Add(glm::vec3(3.f, 1.f, 1.f));

	// Axis-aligned ray along +x; enters box face at x=1
	const S_Ray ray{{0.f, 0.f, 0.f}, {1.f, 0.f, 0.f}};
	const float t = aabb.Intersects(ray);

	EXPECT_NE(t, std::numeric_limits<float>::infinity());
	EXPECT_FLOAT_EQ(t, 1.0f);
}

TEST(AABB_Intersects, Intersects_RayMisses)
{
	S_AABB aabb;
	aabb.Add(glm::vec3(1.f, 1.f, 1.f));
	aabb.Add(glm::vec3(3.f, 3.f, 3.f));

	// Ray travels along +x at y=0, z=0 — misses box which starts at y=1, z=1
	const S_Ray ray{{0.f, 0.f, 0.f}, {1.f, 0.f, 0.f}};
	const float t = aabb.Intersects(ray);

	EXPECT_EQ(t, std::numeric_limits<float>::infinity());
}

TEST(AABB_Intersects, Intersects_RayFromInside)
{
	S_AABB aabb;
	aabb.Add(glm::vec3(-1.f));
	aabb.Add(glm::vec3(1.f));

	// Ray originates inside box — entry t is 0
	const S_Ray ray{{0.f, 0.f, 0.f}, {1.f, 0.f, 0.f}};
	EXPECT_FLOAT_EQ(aabb.Intersects(ray), 0.0f);
}

TEST(AABB_Intersects, Intersects_UninitializedBox)
{
	const S_AABB aabb;
	const S_Ray ray{{0.f, 0.f, 0.f}, {1.f, 0.f, 0.f}};
	EXPECT_EQ(aabb.Intersects(ray), std::numeric_limits<float>::infinity());
}

TEST(AABB_Intersects, Intersects_UninitializedBox_NegativeDirection)
{
	const S_AABB aabb;
	const S_Ray ray{{5.f, 5.f, 5.f}, {-1.f, 0.f, 0.f}};
	EXPECT_EQ(aabb.Intersects(ray), std::numeric_limits<float>::infinity());
}

TEST(AABB_Intersects, Intersects_UninitializedBox_DiagonalDirection)
{
	const S_AABB aabb;
	const glm::vec3 dir = glm::normalize(glm::vec3(1.f, 1.f, 1.f));
	const S_Ray ray{{0.f, 0.f, 0.f}, dir};
	EXPECT_EQ(aabb.Intersects(ray), std::numeric_limits<float>::infinity());
}

TEST(AABB_Intersects, Intersects_UninitializedBox_AxisAlignedAxes)
{
	// Ensure the guard fires for all three principal axes
	const S_AABB aabb;
	EXPECT_EQ(aabb.Intersects(S_Ray{{0.f, 0.f, 0.f}, {0.f, 1.f, 0.f}}), std::numeric_limits<float>::infinity());
	EXPECT_EQ(aabb.Intersects(S_Ray{{0.f, 0.f, 0.f}, {0.f, 0.f, 1.f}}), std::numeric_limits<float>::infinity());
}

// =============================================================================
// IntersectImpl — Graphics Gems ray-box algorithm
// Returns distance to hit point (>0), 0.0 when inside, negative on miss.
// =============================================================================

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
	EXPECT_LT(aabb.IntersectImpl(ray), 0.f);
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
	EXPECT_LT(aabb.IntersectImpl(ray), 0.f);
}

TEST(AABB_IntersectImpl, IntersectImpl_Uninitialized_NegativeDirection)
{
	const S_AABB aabb;
	const S_Ray ray{{5.f, 5.f, 5.f}, {-1.f, 0.f, 0.f}};
	EXPECT_LT(aabb.IntersectImpl(ray), 0.f);
}

TEST(AABB_IntersectImpl, IntersectImpl_Uninitialized_DiagonalDirection)
{
	const S_AABB aabb;
	const glm::vec3 dir = glm::normalize(glm::vec3(1.f, 1.f, 1.f));
	const S_Ray ray{{0.f, 0.f, 0.f}, dir};
	EXPECT_LT(aabb.IntersectImpl(ray), 0.f);
}

TEST(AABB_IntersectImpl, IntersectImpl_Uninitialized_AxisAlignedAxes)
{
	const S_AABB aabb;
	EXPECT_LT(aabb.IntersectImpl(S_Ray{{0.f, 0.f, 0.f}, {0.f, 1.f, 0.f}}), 0.f);
	EXPECT_LT(aabb.IntersectImpl(S_Ray{{0.f, 0.f, 0.f}, {0.f, 0.f, 1.f}}), 0.f);
}

// =============================================================================
// Add(sphere)
// Documents the current behavior: the implementation ignores sphere.m_radius
// and always adds six axis-aligned points at ±1 from the sphere center.
// This is a known refactor target — these tests lock in the current contract.
// =============================================================================

TEST(AABB_AddSphere, AddSphere_HardcodedUnitRadius)
{
	S_AABB aabb;
	// Sphere at origin with a large radius — but the current Add(sphere) impl
	// hardcodes ±1 offsets, so m_radius is effectively ignored.
	const S_Sphere sphere{{0.f, 0.f, 0.f}, 10.f};
	aabb.Add(sphere);

	EXPECT_EQ(aabb.m_Min, glm::vec3(-1.f));
	EXPECT_EQ(aabb.m_Max, glm::vec3(1.f));
}

// =============================================================================
// updateWithTriangle
// =============================================================================

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

} // namespace GLEngine::Physics::Primitives
