#include <RendererStdafx.h>

#include <Renderer/DebugDraw.h>
#include <Renderer/RayCasting/Geometry/BVH.h>
#include <Renderer/RayCasting/Geometry/Trimesh.h>

#include <Physics/GeometryUtils/TriangleIntersect.h>

// clang-format off
RTTR_REGISTRATION
{
	using namespace GLEngine::Renderer;
	rttr::registration::class_<BVH>("BVH")
		.constructor<>()(rttr::policy::ctor::as_raw_ptr)
		.property("Nodes", &BVH::m_Nodes)(rttr::policy::prop::bind_as_ptr)
		.property("LookUpTable", &BVH::m_LookupTable)(rttr::policy::prop::bind_as_ptr);
	
	rttr::registration::class_<BVH::BVHNode>("BVHNode")
		.constructor<>()(rttr::policy::ctor::as_object)
		.property("AABB", &BVH::BVHNode::aabb)
		.property("left", &BVH::BVHNode::left)
		.property("right", &BVH::BVHNode::right)
		.property("firstTrig", &BVH::BVHNode::firstTrig)
		.property("lastTrig", &BVH::BVHNode::lastTrig);
}
// clang-format on

namespace GLEngine::Renderer {

//=================================================================================
BVH::BVH(std::vector<glm::vec3>& storage)
	: m_Storage(&storage)
{
	Build();
}

//=================================================================================
BVH::BVH(C_Trimesh& trimesh)
	: m_Storage(&trimesh.m_Vertices)
{
}

//=================================================================================
BVH::BVH()
	: m_Storage()
{
}

//=================================================================================
BVH::~BVH() = default;

//=================================================================================
void BVH::Build()
{
	if (m_Storage->empty())
		return;

	GLE_ASSERT(m_Nodes.empty(), "Trying to rebuild the BVH without cleaning.");

	auto& root = m_Nodes.emplace_back();
	for (const auto& vertex : *m_Storage)
		root.aabb.Add(vertex);
	root.firstTrig = 0;
	root.lastTrig  = static_cast<unsigned int>(m_Storage->size()) / 3 - 1;
	std::vector<glm::vec3> centroids;
	centroids.reserve(root.NumTrig());
	const auto trigCenter = [](const glm::vec3* triDef) { return (triDef[0] + triDef[1] + triDef[2]) / 3.f; };

	for (unsigned int i = 0; i < m_Storage->size() / 3; ++i)
	{
		const glm::vec3* triDef = &(m_Storage->at(i * 3));
		centroids.emplace_back(trigCenter(triDef));
		m_LookupTable.emplace_back(i);
	}
	SplitBVHNodeNaive(0, 0, centroids);
}

//=================================================================================
unsigned int BVH::ComputeMaxDepth() const
{
	if (m_Nodes.empty())
		return 0;

	std::function<unsigned int(const BVHNode&)> computeDepth = [&](const BVHNode& node) -> unsigned int {
		if (node.IsLeaf())
			return 1;

		unsigned int leftDepth	= 0;
		unsigned int rightDepth = 0;

		if (node.left != s_InvalidBVHNode)
			leftDepth = computeDepth(m_Nodes[node.left]);
		if (node.right != s_InvalidBVHNode)
			rightDepth = computeDepth(m_Nodes[node.right]);

		return 1 + std::max(leftDepth, rightDepth);
	};

	return computeDepth(m_Nodes[0]);
}

//=================================================================================
void BVH::SplitBVHNodeNaive(T_BVHNodeID nodeId, unsigned int level, std::vector<glm::vec3>& centroids)
{
	if (level > s_MaxDepth)
		return;

	// limit nodes that's too small
	if (m_Nodes[nodeId].NumTrig() <= s_MinLeafSize)
		return;

	// try finding better than average
	const float parentCost	= m_Nodes[nodeId].aabb.Area() * static_cast<float>(m_Nodes[nodeId].NumTrig());
	float		bestCost	= std::numeric_limits<float>::max();
	float		bestAverage = 0.f;
	int			bestAxis	= 0;
	for (int axis = 0; axis < 3; ++axis)
	{
		for (unsigned int i = m_Nodes[nodeId].firstTrig; i < m_Nodes[nodeId].lastTrig; ++i)
		{
			const float currentCentroid = centroids[i][axis];
			const float cost			= CalcSAHCost(m_Nodes[nodeId], axis, currentCentroid, centroids);
			if (cost < bestCost)
			{
				bestCost	= cost;
				bestAverage = currentCentroid;
				bestAxis	= axis;
			}
		}
	}
	if (bestCost >= parentCost)
	{
		return;
	}
	m_Nodes.emplace_back();
	const T_BVHNodeID leftNodeId = static_cast<T_BVHNodeID>(m_Nodes.size()) - 1;
	m_Nodes[nodeId].left		 = leftNodeId;
	m_Nodes.emplace_back();
	const T_BVHNodeID rightNodeId = static_cast<T_BVHNodeID>(m_Nodes.size()) - 1;
	m_Nodes[nodeId].right		  = rightNodeId;
	// now I can store references, because I am not adding new elements
	auto&		left  = m_Nodes[leftNodeId];
	auto&		right = m_Nodes[rightNodeId];
	const auto& node  = m_Nodes[nodeId];

	// Partition triangles around the split position
	const unsigned int lastLeftIndex = PartitionTriangles(centroids, node.firstTrig, node.lastTrig, bestAxis, bestAverage);

	left.firstTrig	= node.firstTrig;
	left.lastTrig	= lastLeftIndex;
	right.firstTrig = lastLeftIndex + 1;
	right.lastTrig	= node.lastTrig;

	GLE_ASSERT(left.NumTrig() + right.NumTrig() == node.NumTrig(), "The children nodes triangles sum should be equal to parent trigs.");

	// build AABBs
	for (unsigned int i = left.firstTrig; i < left.lastTrig + 1; ++i)
	{
		const glm::vec3* triDef = GetTriangleDefinition(i);
		left.aabb.Add(triDef[0]);
		left.aabb.Add(triDef[1]);
		left.aabb.Add(triDef[2]);
	}
	for (unsigned int i = right.firstTrig; i < right.lastTrig + 1; ++i)
	{
		const glm::vec3* triDef = GetTriangleDefinition(i);
		right.aabb.Add(triDef[0]);
		right.aabb.Add(triDef[1]);
		right.aabb.Add(triDef[2]);
	}

	SplitBVHNodeNaive(leftNodeId, level + 1, centroids);
	SplitBVHNodeNaive(rightNodeId, level + 1, centroids);
}

//=================================================================================
bool BVH::Intersect(const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection) const
{
	if (m_Nodes.empty())
		return false;
	return IntersectNode(ray, intersection, m_Nodes[0]);
}

//=================================================================================
bool BVH::IntersectNode(const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection, const BVHNode& node) const
{
	// Early out: if ray origin is outside AABB and doesn't intersect it
	if (!node.aabb.Contains(ray.origin) && !node.aabb.Intersects(ray))
	{
		return false;
	}

	if (node.IsLeaf() == false)
	{
		// we can have hit from both sides as sides can overlap
		C_RayIntersection intersections[2];
		bool			  intersectionsResults[2] = {false, false};
		if (node.left != s_InvalidBVHNode)
			intersectionsResults[0] = IntersectNode(ray, intersections[0], m_Nodes[node.left]);
		if (node.right != s_InvalidBVHNode)
			intersectionsResults[1] = IntersectNode(ray, intersections[1], m_Nodes[node.right]);
		// find closer intersection
		if (intersectionsResults[0] && intersectionsResults[1])
		{
			if (intersections[0].GetRayLength() < intersections[1].GetRayLength())
			{
				intersection = intersections[0];
			}
			else
			{
				intersection = intersections[1];
			}
		}
		else if (intersectionsResults[0])
		{
			intersection = intersections[0];
		}
		else if (intersectionsResults[1])
		{
			intersection = intersections[1];
		}
		return intersectionsResults[0] || intersectionsResults[1];
	}
	// we are in the leaf node
	struct S_IntersectionInfo {
		C_RayIntersection intersection;
		float			  t = std::numeric_limits<float>::infinity();

		[[nodiscard]] bool operator<(const S_IntersectionInfo& a) const { return t < a.t; }
	};
	S_IntersectionInfo closestIntersect{.intersection = C_RayIntersection()};

	glm::vec2 barycentric;

	for (unsigned int i = node.firstTrig; i <= node.lastTrig; ++i)
	{
		const glm::vec3* triDef = GetTriangleDefinition(i);
		const auto		 length = Physics::TriangleRayIntersect(triDef, ray, &barycentric);
		if (length > 0.0f)
		{
			if (closestIntersect.t < length)
			{
				continue;
			}
			auto normal = glm::cross(triDef[1] - triDef[0], triDef[2] - triDef[0]);
			// const auto area	  = glm::length(normal) / 2.f;
			normal = glm::normalize(normal);
			C_RayIntersection inter(S_Frame(normal), ray.origin + length * ray.direction, Physics::Primitives::S_Ray(ray));
			inter.SetRayLength(length);

			closestIntersect = {.intersection = inter, .t = length};
		}
	}
	if (std::isinf(closestIntersect.t))
		return false;


	intersection = closestIntersect.intersection;
	return true;
}

//=================================================================================
void BVH::DebugDraw(I_DebugDraw& dd, const glm::mat4& modelMatrix) const
{
	if (m_Nodes.empty())
		return;
	DebugDrawNode(dd, modelMatrix, m_Nodes[0], 0);
}

//=================================================================================
void BVH::DebugDrawNode(I_DebugDraw& dd, const glm::mat4& modelMatrix, const BVHNode& node, unsigned int level) const
{
	// can be simple loop
	static Colours::T_Colour colours[] = {
		Colours::black, Colours::green, Colours::blue, Colours::red, Colours::gray, Colours::purple, Colours::yellow, Colours::white,
	};
	constexpr static auto	 numColours	   = (sizeof(colours) / sizeof(Colours::T_Colour));
	const Colours::T_Colour& currentColour = colours[level < numColours ? level : numColours - 1];
	dd.DrawAABB(node.aabb, currentColour, modelMatrix);
	if (node.left != s_InvalidBVHNode)
		DebugDrawNode(dd, modelMatrix, m_Nodes[node.left], level + 1);
	if (node.right != s_InvalidBVHNode)
		DebugDrawNode(dd, modelMatrix, m_Nodes[node.right], level + 1);
}

//=================================================================================
float BVH::CalcSAHCost(const BVHNode& parent, const unsigned int axis, const float splitPos, std::vector<glm::vec3>& centroids) const
{
	Physics::Primitives::S_AABB left, right;
	unsigned int				leftCount = 0, rightCount = 0;
	for (unsigned int i = parent.firstTrig; i < parent.lastTrig + 1; ++i)
	{
		const glm::vec3* triDef = GetTriangleDefinition(i);
		if (centroids[i][axis] < splitPos)
		{
			left.Add(triDef[0]);
			left.Add(triDef[1]);
			left.Add(triDef[2]);
			++leftCount;
		}
		else
		{
			right.Add(triDef[0]);
			right.Add(triDef[1]);
			right.Add(triDef[2]);
			++rightCount;
		}
	}
	const float cost = leftCount * left.Area() + rightCount * right.Area();
	return cost > 0.f ? cost : std::numeric_limits<float>::max();
}

//=================================================================================
unsigned int BVH::PartitionTriangles(std::vector<glm::vec3>& centroids, unsigned int first, unsigned int last, int axis, float splitPos)
{
	unsigned int left  = first;
	unsigned int right = last;

	while (left < right)
	{
		// Find element on left that belongs on right
		while (left < right && centroids[left][axis] < splitPos)
			++left;

		// Find element on right that belongs on left
		while (left < right && centroids[right][axis] >= splitPos)
			--right;

		if (left < right)
		{
			std::swap(centroids[left], centroids[right]);
			std::swap(m_LookupTable[left], m_LookupTable[right]);
		}
	}

	// Adjust boundary
	if (centroids[left][axis] < splitPos)
		++left;

	return left - 1; // Returns last index of left partition
}

//=================================================================================
const glm::vec3* BVH::GetTriangleDefinition(const unsigned int triangleIndex) const
{
	const auto lookupPosition = m_LookupTable[triangleIndex];
	return &(m_Storage->at(lookupPosition * 3));
}

} // namespace GLEngine::Renderer
