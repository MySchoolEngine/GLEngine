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
	static const BVHNode& GetRoot(const BVH& bvh) { return bvh.m_Nodes[0]; }
	static const BVHNode& GetLeft(const BVH& bvh, const BVHNode& node) { return bvh.m_Nodes[node.left]; }
	static const BVHNode& GetRight(const BVH& bvh, const BVHNode& node) { return bvh.m_Nodes[node.right]; }
};

TEST_F(BVHFixture, BuildOneNode)
{
	std::vector<glm::vec3> vertices = {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
									   glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)};

	BVH bvh(vertices);

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
} // namespace GLEngine::Renderer