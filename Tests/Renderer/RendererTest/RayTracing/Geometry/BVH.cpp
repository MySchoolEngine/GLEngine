#include <RendererTestStdafx.h>

#include <Renderer/RayCasting/Geometry/BVH.h>

namespace GLEngine::Renderer {

class BVHRayTest : public ::testing::TestWithParam<std::tuple<glm::vec3, glm::vec3, bool>> {
protected:
	void SetUp() override
	{
		triangles.emplace_back(0, 0, 0);
		triangles.emplace_back(1, 0, 0);
		triangles.emplace_back(0, 1, 0);
		bvh_ = std::make_unique<BVH>(triangles);
	}

	std::vector<glm::vec3> triangles;
	std::unique_ptr<BVH>   bvh_;
};

TEST_P(BVHRayTest, RayIntersectionBehavior)
{
	auto [origin, direction, shouldHit] = GetParam();

	const Physics::Primitives::S_Ray ray{.origin = origin, .direction = normalize(direction)};
	C_RayIntersection				 hit;
	const bool						 intersected = bvh_->Intersect(ray, hit);

	EXPECT_EQ(shouldHit, intersected) << "Ray: origin=" << origin << " dir=" << direction;

	if (shouldHit && intersected)
	{
		EXPECT_GT(hit.GetRayLength(), 0.0f);
		EXPECT_TRUE(std::isfinite(hit.GetRayLength()));
	}
}

class BVHFixture : public ::testing::Test {
public:
	static std::size_t GetNodesCount(const BVH& bvh) { return bvh.m_Nodes.size(); }

	static std::size_t CountVerticesInLeaves(const BVH& bvh)
	{
		std::size_t count = 0;
		for (const auto& node : bvh.m_Nodes)
		{
			if (node.IsLeaf())
			{
				count += (node.lastTrig - node.firstTrig + 1);
			}
		}
		return count;
	}

	static const auto& GetLookupTable(const BVH& bvh) { return bvh.m_LookupTable; }

	static bool NoDuplicatePrimitives(const BVH& bvh)
	{
		std::set<unsigned int> vertexIndices;

		for (const auto& node : bvh.m_Nodes)
		{
			if (node.IsLeaf())
			{
				for (unsigned int i = node.firstTrig; i <= node.lastTrig; ++i)
				{
					// Each vertex index should appear only once across all leaves
					if (!vertexIndices.insert(i).second)
					{
						return false; // Duplicate found
					}
				}
			}
		}
		return true;
	}

	static bool ParentContainsChildrenBounds(const BVH& bvh)
	{
		for (const auto& node : bvh.m_Nodes)
		{
			if (!node.IsLeaf())
			{
				const auto& leftChild  = bvh.m_Nodes[node.left];
				const auto& rightChild = bvh.m_Nodes[node.right];

				// Check if parent AABB contains left child AABB
				if (!node.aabb.Contains(leftChild.aabb.m_Min) || !node.aabb.Contains(leftChild.aabb.m_Max))
				{
					return false;
				}

				// Check if parent AABB contains right child AABB
				if (!node.aabb.Contains(rightChild.aabb.m_Min) || !node.aabb.Contains(rightChild.aabb.m_Max))
				{
					return false;
				}
			}
		}
		return true;
	}

	static bool LeavesHavePrimitives(const BVH& bvh)
	{
		for (const auto& node : bvh.m_Nodes)
		{
			if (node.IsLeaf())
			{
				if (node.NumTrig() == 0)
				{
					return false; // Empty leaf found
				}
			}
		}
		return true;
	}

	using BVHNode = BVH::BVHNode;
	static const BVHNode&  GetRoot(const BVH& bvh) { return bvh.m_Nodes[0]; }
	static const BVHNode&  GetLeft(const BVH& bvh, const BVHNode& node) { return bvh.m_Nodes[node.left]; }
	static const BVHNode&  GetRight(const BVH& bvh, const BVHNode& node) { return bvh.m_Nodes[node.right]; }
	static constexpr float EPSILON = 1e-5f;
};

TEST_F(BVHFixture, BuildOneNode)
{
	std::vector<glm::vec3> vertices = {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
									   glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)};

	const BVH bvh(vertices);

	EXPECT_EQ(GetNodesCount(bvh), 1);
}

TEST_F(BVHFixture, SAHSelectsMinimumCostSplit)
{
	// Create scene with two distinct clusters
	std::vector<glm::vec3> triangles;
	for (int i = 0; i < 50; i++)
	{
		triangles.emplace_back(i, 0, 0);
		triangles.emplace_back(i + 1, 0, 0);
		triangles.emplace_back(i, 1, 0);
	}
	for (int i = 0; i < 50; i++)
	{
		triangles.emplace_back(i + 100, 0, 0);
		triangles.emplace_back(i + 101, 0, 0);
		triangles.emplace_back(i + 100, 1, 0);
	}

	const BVH bvh(triangles);

	// SAH should split between clusters at root
	const BVHNode& root = GetRoot(bvh);
	ASSERT_FALSE(root.IsLeaf());

	// Verify split occurred between clusters, not within them
	const auto& leftNode	= GetLeft(bvh, root);
	const auto& rightNode	= GetRight(bvh, root);
	const auto& leftBounds	= leftNode.aabb;
	const auto& rightBounds = rightNode.aabb;
	EXPECT_LT(leftBounds.m_Max.x, 90.0f);  // Left cluster in [0, 50]
	EXPECT_GT(rightBounds.m_Min.x, 90.0f); // Right cluster in [100, 150]
	EXPECT_EQ(leftNode.firstTrig, 0);
	EXPECT_EQ(leftNode.lastTrig, 49);
	EXPECT_EQ(rightNode.firstTrig, 50);
	EXPECT_EQ(rightNode.lastTrig, 99);
	for (int i = 0; i < 100; ++i)
	{
		EXPECT_EQ(GetLookupTable(bvh)[i], i) << "No reorganizing needed";
	}
}

TEST_F(BVHFixture, SAHSelectsMinimumCostSplit_NeedsRearrangement)
{
	// Create scene with two distinct clusters
	std::vector<glm::vec3> triangles;
	for (int i = 0; i < 50; i++)
	{
		triangles.emplace_back(150 - i, 0, 0);
		triangles.emplace_back(150 - i + 1, 0, 0);
		triangles.emplace_back(150 - i, 1, 0);
	}
	for (int i = 0; i < 50; i++)
	{
		triangles.emplace_back(50 - i, 0, 0);
		triangles.emplace_back(50 - i + 1, 0, 0);
		triangles.emplace_back(50 - i, 1, 0);
	}

	const BVH bvh(triangles);

	// SAH should split between clusters at root
	const BVHNode& root = GetRoot(bvh);
	ASSERT_FALSE(root.IsLeaf());

	// Verify split occurred between clusters, not within them
	const auto& leftNode	= GetLeft(bvh, root);
	const auto& rightNode	= GetRight(bvh, root);
	const auto& leftBounds	= leftNode.aabb;
	const auto& rightBounds = rightNode.aabb;
	EXPECT_LT(leftBounds.m_Max.x, 90.0f);  // Left cluster in [0, 50]
	EXPECT_GT(rightBounds.m_Min.x, 90.0f); // Right cluster in [100, 150]
	EXPECT_EQ(leftNode.firstTrig, 0);
	EXPECT_EQ(leftNode.lastTrig, 49);
	EXPECT_EQ(rightNode.firstTrig, 50);
	EXPECT_EQ(rightNode.lastTrig, 99);
}

TEST_F(BVHFixture, SAHCreatesLeafForCoincidentCentroids)
{
	// All triangles with same centroid but different extents
	std::vector<glm::vec3> triangles;
	glm::vec3			   center(5, 5, 5);
	for (int i = 0; i < 10; i++)
	{
		float offset = i * 0.1f;
		triangles.push_back(center + glm::vec3(-offset, -offset, 0));
		triangles.push_back(center + glm::vec3(offset, -offset, 0));
		triangles.push_back(center + glm::vec3(0, offset, 0));
	}

	const BVH bvh(triangles);

	// Must terminate and create leaf - cannot split on coincident centroids
	// Tree might be just a root leaf, or have some structure,
	// but should not have infinite recursion
	EXPECT_LT(bvh.ComputeMaxDepth(), 10u); // Reasonable depth
}

INSTANTIATE_TEST_SUITE_P(RayTests,
						 BVHRayTest,
						 ::testing::Values(std::make_tuple(glm::vec3(0.25f, 0.25f, 1.f), glm::vec3(0, 0, -1), true), // hits triangle
										   std::make_tuple(glm::vec3(-1, 2, 0.1f), glm::vec3(1, 0, 0), false)		 // misses above
										   ));

// ============================================================================
// Black Box Ray Intersection Tests
// ============================================================================

TEST_F(BVHFixture, RayParallelToTriangle)
{
	std::vector<glm::vec3> triangles = {glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0)};

	const BVH bvh(triangles);

	// Ray parallel to triangle plane
	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0.5f, 0.5f, 1.0f), .direction = glm::vec3(1, 0, 0)};
	C_RayIntersection					 hit;

	EXPECT_FALSE(bvh.Intersect(ray, hit));
}

TEST_F(BVHFixture, RayHitsTriangleEdge)
{
	std::vector<glm::vec3> triangles = {glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0)};

	const BVH bvh(triangles);

	// Ray hitting the edge between vertices (0,0,0) and (1,0,0)
	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0.5f, 0.0f, 1.0f), .direction = glm::vec3(0, 0, -1)};
	C_RayIntersection					 hit;

	const bool intersected = bvh.Intersect(ray, hit);
	EXPECT_TRUE(intersected);
	if (intersected)
	{
		EXPECT_NEAR(hit.GetRayLength(), 1.0f, EPSILON);
	}
}

TEST_F(BVHFixture, RayHitsTriangleVertex)
{
	std::vector<glm::vec3> triangles = {glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0)};

	const BVH bvh(triangles);

	// Ray hitting vertex (0,0,0)
	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0.0f, 0.0f, 1.0f), .direction = glm::vec3(0, 0, -1)};
	C_RayIntersection					 hit;

	const bool intersected = bvh.Intersect(ray, hit);
	EXPECT_TRUE(intersected);
	if (intersected)
	{
		EXPECT_NEAR(hit.GetRayLength(), 1.0f, EPSILON);
	}
}

TEST_F(BVHFixture, RayStartsInsideBoundingBox)
{
	std::vector<glm::vec3> triangles = {glm::vec3(0, 0, 0), glm::vec3(2, 0, 0), glm::vec3(0, 2, 0)};

	const BVH bvh(triangles);

	// Ray starting inside the bounding box but not hitting the triangle
	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0.5f, 0.5f, 0.5f), .direction = glm::vec3(0, 0, 1)};
	C_RayIntersection					 hit;

	EXPECT_FALSE(bvh.Intersect(ray, hit));
}

TEST_F(BVHFixture, RayOriginOnTriangleSurface)
{
	std::vector<glm::vec3> triangles = {glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0)};

	const BVH bvh(triangles);

	// Ray starting on the triangle surface
	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0.25f, 0.25f, 0.0f), .direction = glm::vec3(0, 0, -1)};
	C_RayIntersection					 hit;

	const bool intersected = bvh.Intersect(ray, hit);
	// Should either hit with distance ~0 or not hit (depends on implementation epsilon handling)
	if (intersected)
	{
		EXPECT_NEAR(hit.GetRayLength(), 0.0f, EPSILON);
	}
}

TEST_F(BVHFixture, MultipleTrianglesAlongRay)
{
	// Two triangles at different depths
	std::vector<glm::vec3> triangles = {// First triangle at z=0
										glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0),
										// Second triangle at z=2
										glm::vec3(0, 0, 2), glm::vec3(1, 0, 2), glm::vec3(0, 1, 2)};

	const BVH bvh(triangles);

	// Ray should hit the first triangle
	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0.25f, 0.25f, -1.0f), .direction = glm::vec3(0, 0, 1)};
	C_RayIntersection					 hit;

	const bool intersected = bvh.Intersect(ray, hit);
	EXPECT_TRUE(intersected);
	if (intersected)
	{
		// Should hit first triangle at distance 1.0
		EXPECT_NEAR(hit.GetRayLength(), 1.0f, EPSILON);
	}
}

TEST_F(BVHFixture, RayFromDifferentDirections)
{
	std::vector<glm::vec3> triangles = {glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0)};

	const BVH bvh(triangles);

	// Test from positive Z
	{
		constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0.25f, 0.25f, 1.0f), .direction = glm::vec3(0, 0, -1)};
		C_RayIntersection					 hit;
		EXPECT_TRUE(bvh.Intersect(ray, hit));
	}

	// Test from negative Z (backface)
	{
		constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0.25f, 0.25f, -1.0f), .direction = glm::vec3(0, 0, 1)};
		C_RayIntersection					 hit;
		// Should hit (backface culling depends on implementation)
		EXPECT_TRUE(bvh.Intersect(ray, hit)); // Just verify it doesn't crash
	}

	// Test from diagonal
	{
		const Physics::Primitives::S_Ray ray{.origin = glm::vec3(-1.0f, -1.0f, 1.0f), .direction = normalize(glm::vec3(1.25f, 1.25f, -1.0f))};
		C_RayIntersection				 hit;
		EXPECT_TRUE(bvh.Intersect(ray, hit));
	}
}

TEST_F(BVHFixture, GrazingAngleRay)
{
	std::vector<glm::vec3> triangles = {glm::vec3(0, 0, 0), glm::vec3(10, 0, 0), glm::vec3(0, 10, 0)};

	const BVH bvh(triangles);

	// Ray at very shallow angle
	const Physics::Primitives::S_Ray ray{.origin = glm::vec3(5.0f, 2.0f, 5.0f), .direction = normalize(glm::vec3(0, 0, -1))};
	C_RayIntersection				 hit;

	const bool intersected = bvh.Intersect(ray, hit);
	EXPECT_TRUE(intersected);
}

// ============================================================================
// BVH Construction Tests
// ============================================================================

TEST_F(BVHFixture, EmptyGeometry)
{
	std::vector<glm::vec3> triangles;

	const BVH bvh(triangles);

	// Should not crash, ray should miss
	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0, 0, 0), .direction = glm::vec3(0, 0, 1)};
	C_RayIntersection					 hit;

	EXPECT_FALSE(bvh.Intersect(ray, hit));
}

TEST_F(BVHFixture, SingleTriangle)
{
	std::vector<glm::vec3> triangles = {glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0)};

	const BVH bvh(triangles);

	// Should intersect
	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0.25f, 0.25f, 1.0f), .direction = glm::vec3(0, 0, -1)};
	C_RayIntersection					 hit;

	EXPECT_TRUE(bvh.Intersect(ray, hit));
}

TEST_F(BVHFixture, CoplanarTriangles)
{
	// Multiple triangles in the same plane
	std::vector<glm::vec3> triangles = {// Triangle 1
										glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0),
										// Triangle 2
										glm::vec3(1, 0, 0), glm::vec3(1, 1, 0), glm::vec3(0, 1, 0),
										// Triangle 3
										glm::vec3(2, 0, 0), glm::vec3(3, 0, 0), glm::vec3(2, 1, 0)};

	const BVH bvh(triangles);

	// Test hitting different triangles
	{
		constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0.25f, 0.25f, 1.0f), .direction = glm::vec3(0, 0, -1)};
		C_RayIntersection					 hit;
		EXPECT_TRUE(bvh.Intersect(ray, hit));
		EXPECT_FLOAT_EQ(hit.GetRayLength(), 1.f);
	}

	{
		constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(2.25f, 0.25f, 1.0f), .direction = glm::vec3(0, 0, -1)};
		C_RayIntersection					 hit;
		EXPECT_TRUE(bvh.Intersect(ray, hit));
		EXPECT_FLOAT_EQ(hit.GetRayLength(), 1.f);
	}
}

TEST_F(BVHFixture, AxisAlignedTriangles)
{
	// Triangle aligned with XY plane
	std::vector<glm::vec3> triangles = {glm::vec3(0, 0, 5), glm::vec3(1, 0, 5), glm::vec3(0, 1, 5)};

	const BVH bvh(triangles);

	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0.25f, 0.25f, 0.0f), .direction = glm::vec3(0, 0, 1)};
	C_RayIntersection					 hit;

	const bool intersected = bvh.Intersect(ray, hit);
	EXPECT_TRUE(intersected);
	if (intersected)
	{
		EXPECT_NEAR(hit.GetRayLength(), 5.0f, EPSILON);
	}
}

TEST_F(BVHFixture, LargeMesh)
{
	// Create a grid of triangles
	std::vector<glm::vec3> triangles;
	for (int x = 0; x < 20; x++)
	{
		for (int y = 0; y < 20; y++)
		{
			// Two triangles per grid cell
			triangles.emplace_back(x, y, 0);
			triangles.emplace_back(x + 1, y, 0);
			triangles.emplace_back(x, y + 1, 0);

			triangles.emplace_back(x + 1, y, 0);
			triangles.emplace_back(x + 1, y + 1, 0);
			triangles.emplace_back(x, y + 1, 0);
		}
	}

	const BVH bvh(triangles);

	// Test intersection at various points
	for (int i = 0; i < 10; i++)
	{
		float							 x = i * 2.0f + 0.5f;
		float							 y = i * 1.5f + 0.5f;
		const Physics::Primitives::S_Ray ray{.origin = glm::vec3(x, y, 1.0f), .direction = glm::vec3(0, 0, -1)};
		C_RayIntersection				 hit;

		EXPECT_TRUE(bvh.Intersect(ray, hit)) << "Failed at position (" << x << ", " << y << ")";
	}
}

// ============================================================================
// Degenerate Case Tests
// ============================================================================

TEST_F(BVHFixture, ZeroAreaTriangle)
{
	// Degenerate triangle with all vertices at same point
	std::vector<glm::vec3> triangles = {glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)};

	const BVH bvh(triangles);

	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0, 0, 1), .direction = glm::vec3(0, 0, -1)};
	C_RayIntersection					 hit;

	// Should not crash, behavior is implementation-defined
	EXPECT_FALSE(bvh.Intersect(ray, hit));
}

TEST_F(BVHFixture, CollinearTriangle)
{
	// Triangle with all vertices on a line (zero area)
	std::vector<glm::vec3> triangles = {glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(2, 0, 0)};

	const BVH bvh(triangles);

	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(1, 0, 1), .direction = glm::vec3(0, 0, -1)};
	C_RayIntersection					 hit;

	// Should not crash, likely won't intersect
	EXPECT_FALSE(bvh.Intersect(ray, hit));
}

TEST_F(BVHFixture, VerySmallTriangle)
{
	// Tiny triangle
	std::vector<glm::vec3> triangles = {glm::vec3(0, 0, 0), glm::vec3(1e-6f, 0, 0), glm::vec3(0, 1e-6f, 0)};

	const BVH bvh(triangles);

	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(5e-7f, 5e-7f, 1.0f), .direction = glm::vec3(0, 0, -1)};
	C_RayIntersection					 hit;

	// Should handle without crashing
	EXPECT_FALSE(bvh.Intersect(ray, hit));
}

TEST_F(BVHFixture, VeryLargeTriangle)
{
	// Huge triangle
	std::vector<glm::vec3> triangles = {glm::vec3(0, 0, 0), glm::vec3(1e6f, 0, 0), glm::vec3(0, 1e6f, 0)};

	const BVH bvh(triangles);

	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(1000, 1000, 1.0f), .direction = glm::vec3(0, 0, -1)};
	C_RayIntersection					 hit;

	const bool intersected = bvh.Intersect(ray, hit);
	EXPECT_TRUE(intersected);
	if (intersected)
	{
		EXPECT_NEAR(hit.GetRayLength(), 1.0f, EPSILON);
	}
}

TEST_F(BVHFixture, ExtremeAspectRatioTriangle)
{
	// Very thin triangle
	std::vector<glm::vec3> triangles = {glm::vec3(0, 0, 0), glm::vec3(100, 0, 0), glm::vec3(0, 0.01f, 0)};

	const BVH bvh(triangles);

	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(50, 0.005f, 1.0f), .direction = glm::vec3(0, 0, -1)};
	C_RayIntersection					 hit;

	EXPECT_TRUE(bvh.Intersect(ray, hit));
}

// ============================================================================
// Depth and Structure Tests
// ============================================================================

TEST_F(BVHFixture, DepthIsReasonableForSmallMesh)
{
	std::vector<glm::vec3> triangles;
	for (int i = 0; i < 10; i++)
	{
		triangles.emplace_back(i, 0, 0);
		triangles.emplace_back(i + 1, 0, 0);
		triangles.emplace_back(i, 1, 0);
	}

	const BVH bvh(triangles);

	const unsigned int depth = bvh.ComputeMaxDepth();
	EXPECT_LT(depth, 20u) << "Depth seems unreasonably large for 10 triangles";
	EXPECT_GT(depth, 0u) << "Depth should be at least 1";
}

TEST_F(BVHFixture, DepthIncreasesWithMeshSize)
{
	// Small mesh
	std::vector<glm::vec3> smallTriangles;
	for (int i = 0; i < 10; i++)
	{
		smallTriangles.emplace_back(i, 0, 0);
		smallTriangles.emplace_back(i + 1, 0, 0);
		smallTriangles.emplace_back(i, 1, 0);
	}
	BVH				   smallBvh(smallTriangles);
	const unsigned int smallDepth = smallBvh.ComputeMaxDepth();

	// Large mesh
	std::vector<glm::vec3> largeTriangles;
	for (int i = 0; i < 100; i++)
	{
		largeTriangles.emplace_back(i, 0, 0);
		largeTriangles.emplace_back(i + 1, 0, 0);
		largeTriangles.emplace_back(i, 1, 0);
	}
	BVH				   largeBvh(largeTriangles);
	const unsigned int largeDepth = largeBvh.ComputeMaxDepth();

	EXPECT_GE(largeDepth, smallDepth) << "Larger mesh should have equal or greater depth";
}

// ============================================================================
// Intersection Accuracy Tests
// ============================================================================

TEST_F(BVHFixture, DistanceAccuracy)
{
	std::vector<glm::vec3> triangles = {glm::vec3(0, 0, 10), glm::vec3(5, 0, 10), glm::vec3(0, 5, 10)};

	const BVH bvh(triangles);

	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(1, 1, 0), .direction = glm::vec3(0, 0, 1)};
	C_RayIntersection					 hit;

	const bool intersected = bvh.Intersect(ray, hit);
	ASSERT_TRUE(intersected);
	EXPECT_NEAR(hit.GetRayLength(), 10.0f, EPSILON) << "Distance to intersection should be exactly 10";
}

TEST_F(BVHFixture, ClosestHitSelection)
{
	// Multiple triangles along ray path
	std::vector<glm::vec3> triangles = {// Closest triangle at z=5
										glm::vec3(-10, -10, 5), glm::vec3(10, -10, 5), glm::vec3(0, 10, 5),
										// Middle triangle at z=10
										glm::vec3(-10, -10, 10), glm::vec3(10, -10, 10), glm::vec3(0, 10, 10),
										// Farthest triangle at z=15
										glm::vec3(-10, -10, 15), glm::vec3(10, -10, 15), glm::vec3(0, 10, 15)};

	const BVH bvh(triangles);

	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0, 0, 0), .direction = glm::vec3(0, 0, 1)};
	C_RayIntersection					 hit;

	const bool intersected = bvh.Intersect(ray, hit);
	ASSERT_TRUE(intersected);
	EXPECT_NEAR(hit.GetRayLength(), 5.0f, EPSILON) << "Should return closest intersection at z=5";
}

TEST_F(BVHFixture, RayMissesAllTriangles)
{
	std::vector<glm::vec3> triangles = {glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0)};

	const BVH bvh(triangles);

	// Ray completely missing the geometry
	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(10, 10, 10), .direction = glm::vec3(1, 0, 0)};
	C_RayIntersection					 hit;

	EXPECT_FALSE(bvh.Intersect(ray, hit));
}

TEST_F(BVHFixture, RayInOppositeDirection)
{
	std::vector<glm::vec3> triangles = {glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0)};

	const BVH bvh(triangles);

	// Ray pointing away from the triangle
	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0.5f, 0.5f, 1.0f), .direction = glm::vec3(0, 0, 1)};
	C_RayIntersection					 hit;

	EXPECT_FALSE(bvh.Intersect(ray, hit));
}

// ============================================================================
// BVH Structure Invariant Tests
// ============================================================================

TEST_F(BVHFixture, AllPrimitivesAreReachable)
{
	std::vector<glm::vec3> triangles;
	for (int i = 0; i < 50; i++)
	{
		triangles.emplace_back(i, 0, 0);
		triangles.emplace_back(i + 1, 0, 0);
		triangles.emplace_back(i, 1, 0);
	}

	const BVH bvh(triangles);

	// All 50 triangles should be in leaves
	EXPECT_EQ(CountVerticesInLeaves(bvh), 50u) << "All triangles must be reachable in leaf nodes";
}

TEST_F(BVHFixture, NoDuplicatePrimitivesInLeaves)
{
	std::vector<glm::vec3> triangles;
	for (int i = 0; i < 30; i++)
	{
		triangles.emplace_back(i, 0, 0);
		triangles.emplace_back(i + 1, 0, 0);
		triangles.emplace_back(i, 1, 0);
	}

	const BVH bvh(triangles);

	EXPECT_TRUE(NoDuplicatePrimitives(bvh)) << "Each primitive should appear in exactly one leaf";
}

TEST_F(BVHFixture, ParentBoundsContainChildren)
{
	std::vector<glm::vec3> triangles;
	// Create scattered triangles to force tree structure
	for (int i = 0; i < 20; i++)
	{
		float x = i * 5.0f;
		float y = (i % 3) * 5.0f;
		triangles.emplace_back(x, y, 0);
		triangles.emplace_back(x + 1, y, 0);
		triangles.emplace_back(x, y + 1, 0);
	}

	const BVH bvh(triangles);

	EXPECT_TRUE(ParentContainsChildrenBounds(bvh)) << "Parent AABBs must fully contain children AABBs";
}

TEST_F(BVHFixture, NoEmptyLeaves)
{
	std::vector<glm::vec3> triangles;
	for (int i = 0; i < 25; i++)
	{
		triangles.emplace_back(i, 0, 0);
		triangles.emplace_back(i + 1, 0, 0);
		triangles.emplace_back(i, 1, 0);
	}

	const BVH bvh(triangles);

	EXPECT_TRUE(LeavesHavePrimitives(bvh)) << "Leaf nodes must contain at least one primitive";
}

TEST_F(BVHFixture, InvariantsHoldForComplexMesh)
{
	// Complex mesh with multiple clusters
	std::vector<glm::vec3> triangles;

	// Cluster 1: bottom-left
	for (int i = 0; i < 10; i++)
	{
		triangles.emplace_back(i, i, 0);
		triangles.emplace_back(i + 1, i, 0);
		triangles.emplace_back(i, i + 1, 0);
	}

	// Cluster 2: top-right
	for (int i = 0; i < 10; i++)
	{
		triangles.emplace_back(100 + i, 100 + i, 0);
		triangles.emplace_back(101 + i, 100 + i, 0);
		triangles.emplace_back(100 + i, 101 + i, 0);
	}

	// Cluster 3: far away
	for (int i = 0; i < 10; i++)
	{
		triangles.emplace_back(-50 + i, 50, 10);
		triangles.emplace_back(-49 + i, 50, 10);
		triangles.emplace_back(-50 + i, 51, 10);
	}

	const BVH bvh(triangles);

	// Verify all invariants
	EXPECT_EQ(CountVerticesInLeaves(bvh), 30u);
	EXPECT_TRUE(NoDuplicatePrimitives(bvh));
	EXPECT_TRUE(ParentContainsChildrenBounds(bvh));
	EXPECT_TRUE(LeavesHavePrimitives(bvh));
}

// ============================================================================
// Stress Tests with Random/Complex Geometry
// ============================================================================

TEST_F(BVHFixture, OverlappingTriangles)
{
	// Multiple triangles in the same location
	std::vector<glm::vec3> triangles;
	for (int i = 0; i < 10; i++)
	{
		// All triangles at same position but slightly offset
		triangles.emplace_back(0, 0, i * 0.1f);
		triangles.emplace_back(1, 0, i * 0.1f);
		triangles.emplace_back(0, 1, i * 0.1f);
	}

	const BVH bvh(triangles);

	// Should build without crashing
	EXPECT_GT(GetNodesCount(bvh), 0u);
	EXPECT_TRUE(NoDuplicatePrimitives(bvh));

	// Ray through the stack should hit the first one
	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0.25f, 0.25f, -1.0f), .direction = glm::vec3(0, 0, 1)};
	C_RayIntersection					 hit;
	EXPECT_TRUE(bvh.Intersect(ray, hit));
}

TEST_F(BVHFixture, TrianglesFormingCube)
{
	// 12 triangles forming a cube (2 per face)
	std::vector<glm::vec3> triangles = {
		// Front face (z=1)
		glm::vec3(0, 0, 1), glm::vec3(1, 0, 1), glm::vec3(0, 1, 1),
		glm::vec3(1, 0, 1), glm::vec3(1, 1, 1), glm::vec3(0, 1, 1),
		// Back face (z=0)
		glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0),
		glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(1, 1, 0),
		// Left face (x=0)
		glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 1),
		// Right face (x=1)
		glm::vec3(1, 0, 0), glm::vec3(1, 1, 0), glm::vec3(1, 0, 1),
		glm::vec3(1, 0, 1), glm::vec3(1, 1, 0), glm::vec3(1, 1, 1),
		// Bottom face (y=0)
		glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 0, 1),
		glm::vec3(1, 0, 0), glm::vec3(1, 0, 1), glm::vec3(0, 0, 1),
		// Top face (y=1)
		glm::vec3(0, 1, 0), glm::vec3(0, 1, 1), glm::vec3(1, 1, 0),
		glm::vec3(1, 1, 0), glm::vec3(0, 1, 1), glm::vec3(1, 1, 1)};

	const BVH bvh(triangles);

	// Verify invariants
	EXPECT_EQ(CountVerticesInLeaves(bvh), 12u);
	EXPECT_TRUE(NoDuplicatePrimitives(bvh));

	// Test rays hitting different faces
	{
		// Ray hitting front face
		constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0.5f, 0.5f, 2.0f), .direction = glm::vec3(0, 0, -1)};
		C_RayIntersection					 hit;
		EXPECT_TRUE(bvh.Intersect(ray, hit));
		EXPECT_NEAR(hit.GetRayLength(), 1.0f, EPSILON);
	}

	{
		// Ray from inside pointing out
		constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0.5f, 0.5f, 0.5f), .direction = glm::vec3(1, 0, 0)};
		C_RayIntersection					 hit;
		EXPECT_TRUE(bvh.Intersect(ray, hit));
	}
}

TEST_F(BVHFixture, HighlyUnbalancedDistribution)
{
	std::vector<glm::vec3> triangles;

	// 1 triangle far away
	triangles.emplace_back(-1000, -1000, -1000);
	triangles.emplace_back(-999, -1000, -1000);
	triangles.emplace_back(-1000, -999, -1000);

	// 99 triangles clustered together
	for (int i = 0; i < 99; i++)
	{
		triangles.emplace_back(i * 0.1f, 0, 0);
		triangles.emplace_back(i * 0.1f + 0.1f, 0, 0);
		triangles.emplace_back(i * 0.1f, 0.1f, 0);
	}

	const BVH bvh(triangles);

	// Should handle without crashing
	EXPECT_EQ(CountVerticesInLeaves(bvh), 100u);
	EXPECT_TRUE(NoDuplicatePrimitives(bvh));

	// Both rays should work
	{
		constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(-1000, -1000, -999), .direction = glm::vec3(0, 0, -1)};
		C_RayIntersection					 hit;
		EXPECT_TRUE(bvh.Intersect(ray, hit));
	}

	{
		constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0.5f, 0.05f, 1.0f), .direction = glm::vec3(0, 0, -1)};
		C_RayIntersection					 hit;
		EXPECT_TRUE(bvh.Intersect(ray, hit));
	}
}

TEST_F(BVHFixture, AllTrianglesOnOneSide)
{
	// All triangles on positive X side
	std::vector<glm::vec3> triangles;
	for (int i = 0; i < 50; i++)
	{
		triangles.emplace_back(100 + i, i, 0);
		triangles.emplace_back(101 + i, i, 0);
		triangles.emplace_back(100 + i, i + 1, 0);
	}

	const BVH bvh(triangles);

	EXPECT_TRUE(NoDuplicatePrimitives(bvh));
	EXPECT_EQ(CountVerticesInLeaves(bvh), 50u);

	// Ray from negative X should miss
	constexpr Physics::Primitives::S_Ray rayMiss{.origin = glm::vec3(-10, 25, 1), .direction = glm::vec3(0, 0, -1)};
	C_RayIntersection					 hitMiss;
	EXPECT_FALSE(bvh.Intersect(rayMiss, hitMiss));

	// Ray from positive X should hit
	constexpr Physics::Primitives::S_Ray rayHit{.origin = glm::vec3(125, 25, 1), .direction = glm::vec3(0, 0, -1)};
	C_RayIntersection					 hit;
	EXPECT_TRUE(bvh.Intersect(rayHit, hit));
}

// ============================================================================
// Spatial Configuration Edge Cases
// ============================================================================

TEST_F(BVHFixture, NegativeCoordinates)
{
	std::vector<glm::vec3> triangles = {glm::vec3(-10, -10, -10), glm::vec3(-9, -10, -10), glm::vec3(-10, -9, -10)};

	const BVH bvh(triangles);

	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(-10, -10, -9), .direction = glm::vec3(0, 0, -1)};
	C_RayIntersection					 hit;

	EXPECT_TRUE(bvh.Intersect(ray, hit));
	EXPECT_NEAR(hit.GetRayLength(), 1.0f, EPSILON);
}

TEST_F(BVHFixture, MixedPositiveNegativeCoordinates)
{
	std::vector<glm::vec3> triangles;

	// Triangles across origin
	for (int i = -10; i < 10; i++)
	{
		triangles.emplace_back(i, i, 0);
		triangles.emplace_back(i + 1, i, 0);
		triangles.emplace_back(i, i + 1, 0);
	}

	const BVH bvh(triangles);

	EXPECT_EQ(CountVerticesInLeaves(bvh), 20u);
	EXPECT_TRUE(NoDuplicatePrimitives(bvh));

	// Test ray through origin
	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(0, 0, 1), .direction = glm::vec3(0, 0, -1)};
	C_RayIntersection					 hit;
	EXPECT_TRUE(bvh.Intersect(ray, hit));
}

TEST_F(BVHFixture, TrianglesAtCoordinateExtremes)
{
	// Test with large but reasonable coordinates
	std::vector<glm::vec3> triangles = {glm::vec3(1e4f, 1e4f, 1e4f), glm::vec3(1e4f + 1, 1e4f, 1e4f), glm::vec3(1e4f, 1e4f + 1, 1e4f)};

	const BVH bvh(triangles);

	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(1e4f + 0.25f, 1e4f + 0.25f, 1e4f + 1.0f), .direction = glm::vec3(0, 0, -1)};
	C_RayIntersection				 hit;

	EXPECT_TRUE(bvh.Intersect(ray, hit));
}

TEST_F(BVHFixture, TrianglesInAllOctants)
{
	std::vector<glm::vec3> triangles;

	// Place one triangle in each octant
	float coords[][3] = {{1, 1, 1}, {-1, 1, 1}, {1, -1, 1}, {1, 1, -1}, {-1, -1, 1}, {-1, 1, -1}, {1, -1, -1}, {-1, -1, -1}};

	for (auto& coord : coords)
	{
		triangles.emplace_back(coord[0], coord[1], coord[2]);
		triangles.emplace_back(coord[0] + 0.5f, coord[1], coord[2]);
		triangles.emplace_back(coord[0], coord[1] + 0.5f, coord[2]);
	}

	const BVH bvh(triangles);

	EXPECT_EQ(CountVerticesInLeaves(bvh), 8u);
	EXPECT_TRUE(NoDuplicatePrimitives(bvh));
	EXPECT_TRUE(ParentContainsChildrenBounds(bvh));
}

// ============================================================================
// Consistency Tests - BVH vs Brute Force
// ============================================================================

class BVHConsistencyTest : public BVHFixture {
protected:
	// Brute force intersection
	static bool BruteForceIntersect(const std::vector<glm::vec3>& triangles, const Physics::Primitives::S_Ray& ray, C_RayIntersection& hit)
	{
		bool  foundHit	   = false;
		float closestDist = std::numeric_limits<float>::max();

		for (size_t i = 0; i < triangles.size(); i += 3)
		{
			const glm::vec3& v0 = triangles[i];
			const glm::vec3& v1 = triangles[i + 1];
			const glm::vec3& v2 = triangles[i + 2];

			// Möller-Trumbore intersection algorithm
			const glm::vec3 edge1 = v1 - v0;
			const glm::vec3 edge2 = v2 - v0;
			const glm::vec3 h	  = glm::cross(ray.direction, edge2);
			const float		a	  = glm::dot(edge1, h);

			if (std::abs(a) < 1e-8f)
				continue; // Parallel

			const float	    f = 1.0f / a;
			const glm::vec3 s = ray.origin - v0;
			const float		u = f * glm::dot(s, h);

			if (u < 0.0f || u > 1.0f)
				continue;

			const glm::vec3 q = glm::cross(s, edge1);
			const float		v = f * glm::dot(ray.direction, q);

			if (v < 0.0f || u + v > 1.0f)
				continue;

			const float t = f * glm::dot(edge2, q);

			if (t > 1e-8f && t < closestDist)
			{
				closestDist = t;
				foundHit	= true;
				hit.SetRayLength(t);
			}
		}

		return foundHit;
	}
};

TEST_F(BVHConsistencyTest, ConsistencyWithBruteForceSimple)
{
	std::vector<glm::vec3> triangles = {glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0),
											glm::vec3(2, 0, 0), glm::vec3(3, 0, 0), glm::vec3(2, 1, 0),
											glm::vec3(0, 2, 0), glm::vec3(1, 2, 0), glm::vec3(0, 3, 0)};

	const BVH bvh(triangles);

	// Test multiple rays
	std::vector<Physics::Primitives::S_Ray> rays = {
		{.origin = glm::vec3(0.25f, 0.25f, 1.0f), .direction = glm::vec3(0, 0, -1)},
		{.origin = glm::vec3(2.25f, 0.25f, 1.0f), .direction = glm::vec3(0, 0, -1)},
		{.origin = glm::vec3(0.25f, 2.25f, 1.0f), .direction = glm::vec3(0, 0, -1)},
		{.origin = glm::vec3(5.0f, 5.0f, 1.0f), .direction = glm::vec3(0, 0, -1)} // miss
	};

	for (const auto& ray : rays)
	{
		C_RayIntersection bvhHit;
		C_RayIntersection bruteHit;

		bool bvhResult	 = bvh.Intersect(ray, bvhHit);
		bool bruteResult = BruteForceIntersect(triangles, ray, bruteHit);

		EXPECT_EQ(bvhResult, bruteResult) << "BVH and brute force should agree on hit/miss";

		if (bvhResult && bruteResult)
		{
			EXPECT_NEAR(bvhHit.GetRayLength(), bruteHit.GetRayLength(), EPSILON) << "Distance should match brute force";
		}
	}
}

TEST_F(BVHConsistencyTest, ConsistencyWithBruteForceComplex)
{
	// Create more complex scene
	std::vector<glm::vec3> triangles;
	for (int x = 0; x < 10; x++)
	{
		for (int y = 0; y < 10; y++)
		{
			const float z = (x + y) * 0.1f; // Varying heights
			triangles.emplace_back(x, y, z);
			triangles.emplace_back(x + 1, y, z);
			triangles.emplace_back(x, y + 1, z);

			triangles.emplace_back(x + 1, y, z);
			triangles.emplace_back(x + 1, y + 1, z);
			triangles.emplace_back(x, y + 1, z);
		}
	}

	const BVH bvh(triangles);

	// Test rays at various positions
	for (int i = 0; i < 20; i++)
	{
		float							 x	 = i * 0.5f + 0.25f;
		float							 y	 = (i % 7) * 0.7f + 0.25f;
		const Physics::Primitives::S_Ray ray = {.origin = glm::vec3(x, y, 10.0f), .direction = glm::vec3(0, 0, -1)};

		C_RayIntersection bvhHit;
		C_RayIntersection bruteHit;

		bool bvhResult	 = bvh.Intersect(ray, bvhHit);
		bool bruteResult = BruteForceIntersect(triangles, ray, bruteHit);

		EXPECT_EQ(bvhResult, bruteResult) << "BVH and brute force disagree at position (" << x << ", " << y << ")";

		if (bvhResult && bruteResult)
		{
			EXPECT_NEAR(bvhHit.GetRayLength(), bruteHit.GetRayLength(), EPSILON * 10.0f) // Slightly larger epsilon for complex scene
				<< "Distance mismatch at position (" << x << ", " << y << ")";
		}
	}
}

TEST_F(BVHConsistencyTest, ShadowRayConsistency)
{
	// Shadow rays just need hit/miss, not distance
	std::vector<glm::vec3> triangles;
	for (int i = 0; i < 20; i++)
	{
		triangles.emplace_back(i, 0, 0);
		triangles.emplace_back(i + 1, 0, 0);
		triangles.emplace_back(i, 1, 0);
	}

	const BVH bvh(triangles);

	// Test shadow rays from various surface points
	for (int i = 0; i < 20; i++)
	{
		// Ray from surface point upward (should miss)
		const Physics::Primitives::S_Ray rayUp = {.origin = glm::vec3(i + 0.25f, 0.25f, 0.0f), .direction = glm::vec3(0, 0, 1)};

		C_RayIntersection bvhHit;
		C_RayIntersection bruteHit;

		EXPECT_EQ(bvh.Intersect(rayUp, bvhHit), BruteForceIntersect(triangles, rayUp, bruteHit));

		// Ray from above downward (should hit)
		const Physics::Primitives::S_Ray rayDown = {.origin = glm::vec3(i + 0.25f, 0.25f, 1.0f), .direction = glm::vec3(0, 0, -1)};

		EXPECT_EQ(bvh.Intersect(rayDown, bvhHit), BruteForceIntersect(triangles, rayDown, bruteHit));
	}
}

// ============================================================================
// Regression Tests for Common BVH Bugs
// ============================================================================

TEST_F(BVHFixture, DoesNotMissTriangleAtBoundary)
{
	// Common bug: triangles at AABB boundary get missed
	std::vector<glm::vec3> triangles = {glm::vec3(0, 0, 0), glm::vec3(10, 0, 0), glm::vec3(0, 10, 0)};

	const BVH bvh(triangles);

	// Test ray hitting exactly at edge
	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(5.0f, 0.0f, 1.0f), .direction = glm::vec3(0, 0, -1)};
	C_RayIntersection					 hit;

	EXPECT_TRUE(bvh.Intersect(ray, hit)) << "Should hit triangle at boundary";
}

TEST_F(BVHFixture, HandlesNearMissCorrectly)
{
	std::vector<glm::vec3> triangles = {glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0)};

	const BVH bvh(triangles);

	// Ray just barely missing the triangle
	constexpr Physics::Primitives::S_Ray ray{.origin = glm::vec3(1.001f, 0.0f, 1.0f), .direction = glm::vec3(0, 0, -1)};
	C_RayIntersection					 hit;

	EXPECT_FALSE(bvh.Intersect(ray, hit)) << "Should miss triangle that's just outside";
}

TEST_F(BVHFixture, DepthIsConsistentAcrossBuilds)
{
	std::vector<glm::vec3> triangles;
	for (int i = 0; i < 30; i++)
	{
		triangles.emplace_back(i, 0, 0);
		triangles.emplace_back(i + 1, 0, 0);
		triangles.emplace_back(i, 1, 0);
	}

	BVH				 bvh1(triangles);
	const unsigned int depth1 = bvh1.ComputeMaxDepth();

	BVH				 bvh2(triangles);
	const unsigned int depth2 = bvh2.ComputeMaxDepth();

	EXPECT_EQ(depth1, depth2) << "Building same geometry should produce same depth";
}

} // namespace GLEngine::Renderer