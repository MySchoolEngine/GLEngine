#include <RendererTestStdafx.h>

#include <Renderer/RayCasting/Geometry/Trimesh.h>
#include <Renderer/RayCasting/RayIntersection.h>
#include <Renderer/RayCasting/Material/DiffuseMaterial.h>

#include <Physics/Primitives/Triangle.h>

#include <glm/gtx/transform.hpp>

namespace GLEngine::Renderer {

// ============================================================================
// Test Fixture
// ============================================================================

class TrimeshFixture : public ::testing::Test {
protected:
	static constexpr float EPSILON = 1e-5f;

	void SetUp() override
	{
		// Create a simple diffuse material for testing
		testMaterial = std::make_unique<C_DiffuseMaterial>(glm::vec3(1.0f, 1.0f, 1.0f));
	}

	void TearDown() override
	{
		testMaterial.reset();
	}

	// Helper to create a simple triangle
	static Physics::Primitives::S_Triangle MakeTriangle(
		const glm::vec3& v0 = glm::vec3(0, 0, 0),
		const glm::vec3& v1 = glm::vec3(1, 0, 0),
		const glm::vec3& v2 = glm::vec3(0, 1, 0))
	{
		auto tri = Physics::Primitives::S_Triangle::Create(v0, v1, v2);
		EXPECT_TRUE(tri.has_value());
		return tri.value();
	}

	// Test material instance
	std::unique_ptr<C_DiffuseMaterial> testMaterial;
};

// ============================================================================
// Constructor and Assignment Tests
// ============================================================================

TEST_F(TrimeshFixture, DefaultConstructor)
{
	C_Trimesh trimesh;

	EXPECT_EQ(trimesh.GetNumTriangles(), 0u);
	EXPECT_FLOAT_EQ(trimesh.Area(), 0.0f);
}

TEST_F(TrimeshFixture, CopyConstructor)
{
	C_Trimesh original;
	auto tri = MakeTriangle();
	original.AddTriangle(tri);

	C_Trimesh copy(original);

	EXPECT_EQ(copy.GetNumTriangles(), original.GetNumTriangles());
	EXPECT_EQ(copy.GetNumTriangles(), 1u);
}

TEST_F(TrimeshFixture, MoveConstructor)
{
	C_Trimesh original;
	auto tri = MakeTriangle();
	original.AddTriangle(tri);
	original.AddTriangle(tri);

	C_Trimesh moved(std::move(original));

	EXPECT_EQ(moved.GetNumTriangles(), 2u);
	// Original should be in valid but unspecified state
}

TEST_F(TrimeshFixture, CopyAssignment)
{
	C_Trimesh original;
	auto tri = MakeTriangle();
	original.AddTriangle(tri);

	C_Trimesh copy;
	copy = original;

	EXPECT_EQ(copy.GetNumTriangles(), 1u);
	EXPECT_EQ(original.GetNumTriangles(), 1u);
}

TEST_F(TrimeshFixture, MoveAssignment)
{
	C_Trimesh original;
	auto tri = MakeTriangle();
	original.AddTriangle(tri);
	original.AddTriangle(tri);

	C_Trimesh moved;
	moved = std::move(original);

	EXPECT_EQ(moved.GetNumTriangles(), 2u);
}

TEST_F(TrimeshFixture, SelfAssignment)
{
	C_Trimesh trimesh;
	auto tri = MakeTriangle();
	trimesh.AddTriangle(tri);

	trimesh = trimesh; // Self-assignment

	EXPECT_EQ(trimesh.GetNumTriangles(), 1u);
}

// ============================================================================
// AddTriangle Tests
// ============================================================================

TEST_F(TrimeshFixture, AddSingleTriangle)
{
	C_Trimesh trimesh;
	auto tri = MakeTriangle();

	trimesh.AddTriangle(tri);

	EXPECT_EQ(trimesh.GetNumTriangles(), 1u);
}

TEST_F(TrimeshFixture, AddMultipleTriangles)
{
	C_Trimesh trimesh;
	auto tri1 = MakeTriangle(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));
	auto tri2 = MakeTriangle(glm::vec3(1, 0, 0), glm::vec3(1, 1, 0), glm::vec3(0, 1, 0));

	trimesh.AddTriangle(tri1);
	trimesh.AddTriangle(tri2);

	EXPECT_EQ(trimesh.GetNumTriangles(), 2u);
}

TEST_F(TrimeshFixture, AddTriangleWithUV)
{
	C_Trimesh trimesh;
	auto tri = MakeTriangle();
	std::array<glm::vec2, 3> uvs = {glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(0, 1)};

	trimesh.AddTriangle(tri, uvs);

	EXPECT_EQ(trimesh.GetNumTriangles(), 1u);
}

TEST_F(TrimeshFixture, AddMultipleTrianglesWithUV)
{
	C_Trimesh trimesh;
	auto tri = MakeTriangle();
	std::array<glm::vec2, 3> uvs = {glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(0, 1)};

	trimesh.AddTriangle(tri, uvs);
	trimesh.AddTriangle(tri, uvs);
	trimesh.AddTriangle(tri, uvs);

	EXPECT_EQ(trimesh.GetNumTriangles(), 3u);
}

TEST_F(TrimeshFixture, AABBUpdatesWithTriangles)
{
	C_Trimesh trimesh;
	auto tri = MakeTriangle(glm::vec3(1, 1, 1), glm::vec3(5, 1, 1), glm::vec3(1, 5, 1));

	trimesh.AddTriangle(tri);

	const auto& aabb = trimesh.GetAABB();
	EXPECT_GE(aabb.m_Max.x, 5.0f);
	EXPECT_GE(aabb.m_Max.y, 5.0f);
	EXPECT_LE(aabb.m_Min.x, 1.0f);
	EXPECT_LE(aabb.m_Min.y, 1.0f);
}

// ============================================================================
// Ray Intersection Tests (Without Transform)
// ============================================================================

TEST_F(TrimeshFixture, RayIntersectsSingleTriangle)
{
	C_Trimesh trimesh;
	auto tri = MakeTriangle();
	trimesh.AddTriangle(tri);
	trimesh.SetMaterial(testMaterial.get());

	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0.25f, 0.25f, 1.0f), .direction = glm::vec3(0, 0, -1)};
	C_RayIntersection hit;

	EXPECT_TRUE(trimesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
	EXPECT_GT(hit.GetRayLength(), 0.0f);
	EXPECT_TRUE(std::isfinite(hit.GetRayLength()));
}

TEST_F(TrimeshFixture, RayMissesSingleTriangle)
{
	C_Trimesh trimesh;
	auto tri = MakeTriangle();
	trimesh.AddTriangle(tri);

	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(10.0f, 10.0f, 1.0f), .direction = glm::vec3(0, 0, -1)};
	C_RayIntersection hit;

	EXPECT_FALSE(trimesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
}

TEST_F(TrimeshFixture, RayIntersectsClosestTriangle)
{
	C_Trimesh trimesh;

	// Add two triangles at different z depths
	auto tri1 = MakeTriangle(glm::vec3(0, 0, 0), glm::vec3(2, 0, 0), glm::vec3(0, 2, 0));
	auto tri2 = MakeTriangle(glm::vec3(0, 0, 2), glm::vec3(2, 0, 2), glm::vec3(0, 2, 2));

	trimesh.AddTriangle(tri1);
	trimesh.AddTriangle(tri2);
	trimesh.SetMaterial(testMaterial.get());

	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0.5f, 0.5f, -1.0f), .direction = glm::vec3(0, 0, 1)};
	C_RayIntersection hit;

	const bool intersected = trimesh.Intersect(ray, hit, std::numeric_limits<float>::infinity());
	ASSERT_TRUE(intersected);
	// Should hit the closer triangle at z=0
	EXPECT_NEAR(hit.GetRayLength(), 1.0f, EPSILON);
}

TEST_F(TrimeshFixture, RayIntersectionDistance)
{
	C_Trimesh trimesh;
	auto tri = MakeTriangle(glm::vec3(0, 0, 5), glm::vec3(1, 0, 5), glm::vec3(0, 1, 5));
	trimesh.AddTriangle(tri);
	trimesh.SetMaterial(testMaterial.get());

	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0.25f, 0.25f, 0.0f), .direction = glm::vec3(0, 0, 1)};
	C_RayIntersection hit;

	const bool intersected = trimesh.Intersect(ray, hit, std::numeric_limits<float>::infinity());
	ASSERT_TRUE(intersected);
	EXPECT_NEAR(hit.GetRayLength(), 5.0f, EPSILON);
}

TEST_F(TrimeshFixture, RayIntersectionWithUVInterpolation)
{
	C_Trimesh trimesh;
	auto tri = MakeTriangle();
	std::array<glm::vec2, 3> uvs = {glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(0, 1)};

	trimesh.AddTriangle(tri, uvs);
	trimesh.SetMaterial(testMaterial.get());

	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0.5f, 0.0f, 1.0f), .direction = glm::vec3(0, 0, -1)};
	C_RayIntersection hit;

	const bool intersected = trimesh.Intersect(ray, hit, std::numeric_limits<float>::infinity());
	ASSERT_TRUE(intersected);

	const auto uv = hit.GetUV();
	// Ray hits at (0.5, 0, 0), should interpolate UV to approximately (0.5, 0)
	EXPECT_NEAR(uv.x, 0.5f, 0.1f);
	EXPECT_NEAR(uv.y, 0.0f, 0.1f);
}

TEST_F(TrimeshFixture, EmptyTrimeshDoesNotIntersect)
{
	C_Trimesh trimesh;

	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0, 0, 0), .direction = glm::vec3(0, 0, 1)};
	C_RayIntersection hit;

	EXPECT_FALSE(trimesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
}

TEST_F(TrimeshFixture, RayAABBCulling)
{
	C_Trimesh trimesh;
	auto tri = MakeTriangle();
	trimesh.AddTriangle(tri);

	// Ray that misses AABB entirely
	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(-10, -10, 1), .direction = glm::vec3(0, 0, -1)};
	C_RayIntersection hit;

	EXPECT_FALSE(trimesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
}

// ============================================================================
// Transformation Tests
// ============================================================================

TEST_F(TrimeshFixture, SetTransformationTranslation)
{
	C_Trimesh trimesh;
	auto tri = MakeTriangle();
	trimesh.AddTriangle(tri);
	trimesh.SetMaterial(testMaterial.get());

	// Translate by (5, 5, 5)
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(5, 5, 5));
	trimesh.SetTransformation(transform);

	// Ray should now hit at translated position
	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(5.25f, 5.25f, 6.0f), .direction = glm::vec3(0, 0, -1)};
	C_RayIntersection hit;

	EXPECT_TRUE(trimesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
}

TEST_F(TrimeshFixture, SetTransformationScale)
{
	C_Trimesh trimesh;
	auto tri = MakeTriangle();
	trimesh.AddTriangle(tri);
	trimesh.SetMaterial(testMaterial.get());

	// Scale by 2x
	glm::mat4 transform = glm::scale(glm::mat4(1.0f), glm::vec3(2, 2, 2));
	trimesh.SetTransformation(transform);

	// Triangle is now twice as large
	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0.5f, 0.5f, 1.0f), .direction = glm::vec3(0, 0, -1)};
	C_RayIntersection hit;

	EXPECT_TRUE(trimesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
}

TEST_F(TrimeshFixture, SetTransformationUpdatesAABB)
{
	C_Trimesh trimesh;
	auto tri = MakeTriangle();
	trimesh.AddTriangle(tri);

	const auto aabbBefore = trimesh.GetAABB();

	// Translate by (10, 10, 10)
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(10, 10, 10));
	trimesh.SetTransformation(transform);

	const auto& aabbAfter = trimesh.GetAABB();

	// AABB should be translated
	EXPECT_GT(aabbAfter.m_Min.x, aabbBefore.m_Min.x);
	EXPECT_GT(aabbAfter.m_Min.y, aabbBefore.m_Min.y);
	EXPECT_GT(aabbAfter.m_Min.z, aabbBefore.m_Min.z);
}

TEST_F(TrimeshFixture, TransformedRayIntersectionDistance)
{
	C_Trimesh trimesh;
	auto tri = MakeTriangle(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));
	trimesh.AddTriangle(tri);
	trimesh.SetMaterial(testMaterial.get());

	// Translate triangle to z=5
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 5));
	trimesh.SetTransformation(transform);

	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0.25f, 0.25f, 0.0f), .direction = glm::vec3(0, 0, 1)};
	C_RayIntersection hit;

	const bool intersected = trimesh.Intersect(ray, hit, std::numeric_limits<float>::infinity());
	ASSERT_TRUE(intersected);
	EXPECT_NEAR(hit.GetRayLength(), 5.0f, EPSILON);
}

// ============================================================================
// BVH Integration Tests
// ============================================================================

TEST_F(TrimeshFixture, SetBVH)
{
	C_Trimesh trimesh;
	auto tri = MakeTriangle();
	trimesh.AddTriangle(tri);
	trimesh.SetMaterial(testMaterial.get());

	// Note: BVH is managed externally, we just test that setter doesn't crash
	trimesh.SetBVH(nullptr);

	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0.25f, 0.25f, 1.0f), .direction = glm::vec3(0, 0, -1)};
	C_RayIntersection hit;

	// Should still work without BVH (falls back to brute force)
	EXPECT_TRUE(trimesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
}

// ============================================================================
// GetNumTriangles Tests
// ============================================================================

TEST_F(TrimeshFixture, GetNumTrianglesEmpty)
{
	C_Trimesh trimesh;
	EXPECT_EQ(trimesh.GetNumTriangles(), 0u);
}

TEST_F(TrimeshFixture, GetNumTrianglesMultiple)
{
	C_Trimesh trimesh;
	auto tri = MakeTriangle();

	for (int i = 0; i < 10; ++i)
	{
		trimesh.AddTriangle(tri);
	}

	EXPECT_EQ(trimesh.GetNumTriangles(), 10u);
}

// ============================================================================
// Edge Cases and Degenerate Triangles
// ============================================================================

TEST_F(TrimeshFixture, VeryLargeTriangle)
{
	C_Trimesh trimesh;
	auto tri = MakeTriangle(glm::vec3(0, 0, 0), glm::vec3(1e6f, 0, 0), glm::vec3(0, 1e6f, 0));

	trimesh.AddTriangle(tri);
	trimesh.SetMaterial(testMaterial.get());

	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(1000, 1000, 1.0f), .direction = glm::vec3(0, 0, -1)};
	C_RayIntersection hit;

	EXPECT_TRUE(trimesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
}

TEST_F(TrimeshFixture, VerySmallTriangle)
{
	C_Trimesh trimesh;
	auto tri = MakeTriangle(glm::vec3(0, 0, 0), glm::vec3(1e-3f, 0, 0), glm::vec3(0, 1e-3f, 0));

	trimesh.AddTriangle(tri);
	trimesh.SetMaterial(testMaterial.get());

	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(5e-7f, 5e-7f, 1.0f), .direction = glm::vec3(0, 0, -1)};
	C_RayIntersection hit;

	// Might or might not hit depending on precision
	std::ignore = trimesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()); // Just verify it doesn't crash
}

// ============================================================================
// Multiple Triangles Tests
// ============================================================================

TEST_F(TrimeshFixture, MultipleTrianglesFormingQuad)
{
	C_Trimesh trimesh;

	// Two triangles forming a quad
	auto tri1 = MakeTriangle(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));
	auto tri2 = MakeTriangle(glm::vec3(1, 0, 0), glm::vec3(1, 1, 0), glm::vec3(0, 1, 0));

	trimesh.AddTriangle(tri1);
	trimesh.AddTriangle(tri2);
	trimesh.SetMaterial(testMaterial.get());

	EXPECT_EQ(trimesh.GetNumTriangles(), 2u);

	// Ray through first triangle
	{
		constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0.25f, 0.25f, 1.0f), .direction = glm::vec3(0, 0, -1)};
		C_RayIntersection hit;
		EXPECT_TRUE(trimesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
	}

	// Ray through second triangle
	{
		constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0.75f, 0.75f, 1.0f), .direction = glm::vec3(0, 0, -1)};
		C_RayIntersection hit;
		EXPECT_TRUE(trimesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
	}
}

TEST_F(TrimeshFixture, ManyTriangles)
{
	C_Trimesh trimesh;

	// Add 100 triangles
	for (int i = 0; i < 100; ++i)
	{
		auto tri = MakeTriangle(
			glm::vec3(i, 0, 0),
			glm::vec3(i + 1, 0, 0),
			glm::vec3(i, 1, 0)
		);
		trimesh.AddTriangle(tri);
	}
	trimesh.SetMaterial(testMaterial.get());

	EXPECT_EQ(trimesh.GetNumTriangles(), 100u);

	// Test intersection at various positions
	for (int i = 0; i < 10; ++i)
	{
		const Physics::Primitives::S_Ray ray{
			.origin = glm::vec3(i * 10.0f + 0.25f, 0.25f, 1.0f),
			.direction = glm::vec3(0, 0, -1)
		};
		C_RayIntersection hit;

		EXPECT_TRUE(trimesh.Intersect(ray, hit, std::numeric_limits<float>::infinity())) << "Failed to hit triangle at position " << i * 10;
	}
}

// ============================================================================
// Material Tests
// ============================================================================

TEST_F(TrimeshFixture, MaterialIsSetOnIntersection)
{
	C_Trimesh trimesh;
	auto tri = MakeTriangle();
	trimesh.AddTriangle(tri);
	trimesh.SetMaterial(testMaterial.get());

	// Material should be set through I_RayGeometryObject interface
	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0.25f, 0.25f, 1.0f), .direction = glm::vec3(0, 0, -1)};
	C_RayIntersection hit;

	const bool intersected = trimesh.Intersect(ray, hit, std::numeric_limits<float>::infinity());
	ASSERT_TRUE(intersected);

	// Material should be set (even if nullptr)
	EXPECT_NE(hit.GetMaterial(), nullptr) << "Material should be set on intersection";
}

// ============================================================================
// Ray Direction Edge Cases
// ============================================================================

TEST_F(TrimeshFixture, RayParallelToTriangle)
{
	C_Trimesh trimesh;
	auto tri = MakeTriangle();
	trimesh.AddTriangle(tri);

	// Ray parallel to triangle plane
	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0.5f, 0.5f, 1.0f), .direction = glm::vec3(1, 0, 0)};
	C_RayIntersection hit;

	EXPECT_FALSE(trimesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
}

TEST_F(TrimeshFixture, RayFromBehindTriangle)
{
	C_Trimesh trimesh;
	auto tri = MakeTriangle();
	trimesh.AddTriangle(tri);

	// Ray pointing away from triangle
	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0.5f, 0.5f, 1.0f), .direction = glm::vec3(0, 0, 1)};
	C_RayIntersection hit;

	EXPECT_FALSE(trimesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
}

TEST_F(TrimeshFixture, RayHitsTriangleEdge)
{
	C_Trimesh trimesh;
	auto tri = MakeTriangle();
	trimesh.AddTriangle(tri);
	trimesh.SetMaterial(testMaterial.get());

	// Ray hitting edge between (0,0,0) and (1,0,0)
	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0.5f, 0.0f, 1.0f), .direction = glm::vec3(0, 0, -1)};
	C_RayIntersection hit;

	const bool intersected = trimesh.Intersect(ray, hit, std::numeric_limits<float>::infinity());
	EXPECT_TRUE(intersected);
	if (intersected)
	{
		EXPECT_NEAR(hit.GetRayLength(), 1.0f, EPSILON);
	}
}

TEST_F(TrimeshFixture, RayHitsTriangleVertex)
{
	C_Trimesh trimesh;
	auto tri = MakeTriangle();
	trimesh.AddTriangle(tri);
	trimesh.SetMaterial(testMaterial.get());

	// Ray hitting vertex (0,0,0)
	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0.0f, 0.0f, 1.0f), .direction = glm::vec3(0, 0, -1)};
	C_RayIntersection hit;

	const bool intersected = trimesh.Intersect(ray, hit, std::numeric_limits<float>::infinity());
	EXPECT_TRUE(intersected);
	if (intersected)
	{
		EXPECT_NEAR(hit.GetRayLength(), 1.0f, EPSILON);
	}
}

// ============================================================================
// Negative Coordinates Tests
// ============================================================================

TEST_F(TrimeshFixture, NegativeCoordinates)
{
	C_Trimesh trimesh;
	auto tri = MakeTriangle(glm::vec3(-10, -10, -10), glm::vec3(-9, -10, -10), glm::vec3(-10, -9, -10));

	trimesh.AddTriangle(tri);
	trimesh.SetMaterial(testMaterial.get());

	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(-10, -10, -9), .direction = glm::vec3(0, 0, -1)};
	C_RayIntersection hit;

	EXPECT_TRUE(trimesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
}

TEST_F(TrimeshFixture, MixedPositiveNegativeCoordinates)
{
	C_Trimesh trimesh;

	for (int i = -5; i < 5; ++i)
	{
		auto tri = MakeTriangle(
			glm::vec3(i, i, 0),
			glm::vec3(i + 1, i, 0),
			glm::vec3(i, i + 1, 0)
		);
		trimesh.AddTriangle(tri);
	}
	trimesh.SetMaterial(testMaterial.get());

	EXPECT_EQ(trimesh.GetNumTriangles(), 10u);

	// Test ray through origin
	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0, 0, 1), .direction = glm::vec3(0, 0, -1)};
	C_RayIntersection hit;
	EXPECT_TRUE(trimesh.Intersect(ray, hit, std::numeric_limits<float>::infinity()));
}

// ============================================================================
// Consistency Tests
// ============================================================================

TEST_F(TrimeshFixture, CopyHasSameIntersectionBehavior)
{
	C_Trimesh original;
	auto tri = MakeTriangle();
	original.AddTriangle(tri);
	original.SetMaterial(testMaterial.get());

	C_Trimesh copy(original);

	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0.25f, 0.25f, 1.0f), .direction = glm::vec3(0, 0, -1)};

	C_RayIntersection hitOriginal;
	C_RayIntersection hitCopy;

	const bool intersectedOriginal = original.Intersect(ray, hitOriginal, std::numeric_limits<float>::infinity());
	const bool intersectedCopy	   = copy.Intersect(ray, hitCopy, std::numeric_limits<float>::infinity());

	EXPECT_EQ(intersectedOriginal, intersectedCopy);
	if (intersectedOriginal && intersectedCopy)
	{
		EXPECT_NEAR(hitOriginal.GetRayLength(), hitCopy.GetRayLength(), EPSILON);
	}
}

} // namespace GLEngine::Renderer
