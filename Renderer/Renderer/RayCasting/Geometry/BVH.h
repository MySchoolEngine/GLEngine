#pragma once

#include <Renderer/RayCasting/RayIntersection.h>

#include <Physics/Primitives/AABB.h>

#include <rttr/registration_friend.h>

namespace GLEngine::Renderer {
class I_DebugDraw;
class C_Trimesh;

/**
 * BVH is either loaded from file or built.
 * 1) Load file - In that case we need to set vertices from trimesh
 * 2) Build		- First set the pointer to the BVH and lately explicitly build
 */
class RENDERER_API_EXPORT BVH final {
public:
	// BVH is not owning struct, but can modify order of elements in the storage
	explicit BVH(std::vector<glm::vec3>& storage);
	explicit BVH(C_Trimesh& trimesh);
	BVH(); // ONLY for deserialization
	~BVH();
	[[nodiscard]] bool Intersect(const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection) const;

	void DebugDraw(I_DebugDraw& dd, const glm::mat4& modelMatrix) const;

	void Build();

	unsigned int ComputeMaxDepth() const;

	RTTR_REGISTRATION_FRIEND

private:
	using T_BVHNodeID							  = unsigned short;
	constexpr static T_BVHNodeID s_InvalidBVHNode = static_cast<T_BVHNodeID>(-1);

	struct BVHNode final {
		Physics::Primitives::S_AABB aabb;
		T_BVHNodeID					left  = s_InvalidBVHNode;
		T_BVHNodeID					right = s_InvalidBVHNode;
		unsigned int				firstTrig, lastTrig; // Index to look-up table

		[[nodiscard]] bool IsLeaf() const { return left == s_InvalidBVHNode && right == s_InvalidBVHNode; }

		[[nodiscard]] constexpr unsigned int NumTrig() const { return (lastTrig - firstTrig) + 1; }
	};
	[[nodiscard]] bool IntersectNode(const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection, const BVHNode& node) const;
	void			   DebugDrawNode(I_DebugDraw& dd, const glm::mat4& modelMatrix, const BVHNode& node, unsigned int level) const;
	// using NodeID because the vector is being reallocated on the way
	void SplitBVHNodeNaive(T_BVHNodeID node, unsigned int level, std::vector<glm::vec3>& centroids);

	[[nodiscard]] float CalcSAHCost(const BVHNode& parent, const unsigned int axis, const float splitPos, std::vector<glm::vec3>& centroids) const;

	[[nodiscard]] unsigned int PartitionTriangles(std::vector<glm::vec3>& centroids, unsigned int first, unsigned int last, int axis, float splitPos);

	/**
	 * @brief
	 * @param triangleIndex index into the lookup table
	 * @return
	 */
	[[nodiscard]] const glm::vec3* GetTriangleDefinition(const unsigned int triangleIndex) const;

	std::vector<glm::vec3>*	  m_Storage;
	std::vector<unsigned int> m_LookupTable; // index to triangle, to get first vertex multiply * 3
	std::vector<BVHNode>	  m_Nodes;

	static constexpr unsigned int s_MaxDepth	= 10;
	static constexpr unsigned int s_MinLeafSize = 20;

	friend class C_TrimeshModel;
	friend class BVHFixture;
};
} // namespace GLEngine::Renderer
