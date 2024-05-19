#include <RendererStdafx.h>

#include <Renderer/DebugDraw.h>
#include <Renderer/RayCasting/Geometry/BVH.h>
#include <Renderer/RayCasting/Geometry/Trimesh.h>

#include <Physics/GeometryUtils/TriangleIntersect.h>

namespace GLEngine::Renderer {

//=================================================================================
BVH::BVH(std::vector<glm::vec3>& storage)
	: m_Storage(storage)
{
	Build();
}

//=================================================================================
BVH::BVH(C_Trimesh& trimesh)
	: m_Storage(trimesh.m_Vertices)
{
}

//=================================================================================
BVH::~BVH() = default;

//=================================================================================
void BVH::Build()
{
	if (m_Storage.empty())
		return;

	GLE_ASSERT(m_Nodes.empty(), "Trying to rebuild the BVH without cleaning.");

	auto& root = m_Nodes.emplace_back();
	for (const auto& vertex : m_Storage)
		root.aabb.Add(vertex);
	root.firstTrig = 0;
	root.lastTrig  = static_cast<unsigned int>(m_Storage.size()) - 3;
	std::vector<glm::vec3> centroids;
	centroids.reserve(root.NumTrig());
	const auto			   trigCenter = [](const glm::vec3* triDef) { return (triDef[0] + triDef[1] + triDef[2]) / 3.f; };

	for (unsigned int i = root.firstTrig; i < root.lastTrig + 3; i += 3)
	{
		const glm::vec3* triDef = &(m_Storage[i]);
		centroids.emplace_back(trigCenter(triDef));
	}
	SplitBVHNodeNaive(0, 0, centroids);
}

//=================================================================================
void BVH::SplitBVHNodeNaive(T_BVHNodeID nodeId, unsigned int level, std::vector<glm::vec3>& centroids)
{
	if (level > 10)
		return;

	// limit nodes thats too small
	if (m_Nodes[nodeId].NumTrig() <= 20)
		return;

	// try finding better than average
	const float	 parentCost	 = m_Nodes[nodeId].aabb.Area() * m_Nodes[nodeId].NumTrig();
	float		 bestCost	 = std::numeric_limits<float>::max();
	float		 bestAverage = 0.f;
	unsigned int bestAxis	 = 0;
	unsigned int bestTrig	 = 0;
	for (unsigned int axe = 0; axe < 3; ++axe)
	{
		for (unsigned int i = m_Nodes[nodeId].firstTrig; i < m_Nodes[nodeId].lastTrig + 3; i += 3)
		{
			const glm::vec3* triDef			 = &(m_Storage[i]);
			const float		 currentCentroid = centroids[i/3][axe];
			const float		 cost			 = CalcSAHCost(m_Nodes[nodeId], axe, currentCentroid, centroids);
			if (cost < bestCost)
			{
				bestCost	= cost;
				bestAverage = currentCentroid;
				bestAxis	= axe;
				bestTrig	= i;
			}
		}
	}
	if (bestCost >= parentCost) {
		return;
	}
	m_Nodes.emplace_back();
	const T_BVHNodeID leftNodeId = static_cast<T_BVHNodeID>(m_Nodes.size()) - 1;
	m_Nodes[nodeId].left		 = leftNodeId;
	m_Nodes.emplace_back();
	const T_BVHNodeID rightNodeId = static_cast<T_BVHNodeID>(m_Nodes.size()) - 1;
	m_Nodes[nodeId].right		  = rightNodeId;
	// now I can store references, because I am not adding new elements
	auto&			   left	   = m_Nodes[leftNodeId];
	auto&			   right   = m_Nodes[rightNodeId];
	auto&			   node	   = m_Nodes[nodeId];

	const unsigned int axis = bestAxis;
	const float average = bestAverage;

	unsigned int leftSorting = node.firstTrig, rightSorting = node.lastTrig;
	while (leftSorting < rightSorting)
	{
		// find left candidate to swap
		if (centroids[leftSorting / 3][axis] < average)
		{
			leftSorting += 3;
			continue;
		}
		// find right candidate for swap
		while (true)
		{
			if (centroids[rightSorting / 3][axis] < average)
			{
				break;
			}
			rightSorting -= 3;
		}
		if (leftSorting < rightSorting)
		{
			// swap
			GLE_TODO("19-05-2024", "RohacekD", "Needs to scrumble ather attributes as well.");
			std::swap(m_Storage[leftSorting], m_Storage[rightSorting]);
			std::swap(m_Storage[leftSorting + 1], m_Storage[rightSorting + 1]);
			std::swap(m_Storage[leftSorting + 2], m_Storage[rightSorting + 2]);
			std::swap(centroids[leftSorting / 3], centroids[rightSorting / 3]);
		}
	}
	if (leftSorting >= rightSorting)
	{
		if (centroids[leftSorting / 3][axis] < average)
		{
			rightSorting += 3;
		}
		else
		{
			leftSorting -= 3;
		}
	}

	left.firstTrig	= node.firstTrig;
	left.lastTrig	= leftSorting;
	right.firstTrig = leftSorting + 3; // not using rightSorting to count in all triangles
	right.lastTrig	= node.lastTrig;

	GLE_ASSERT(left.NumTrig() + right.NumTrig() == node.NumTrig(), "The children nodes triangels sum should be equal to parent trigs.");

	// build AABBs
	for (unsigned int i = left.firstTrig; i < left.lastTrig + 3; ++i)
	{
		left.aabb.Add(m_Storage[i]);
	}
	for (unsigned int i = right.firstTrig; i < right.lastTrig + 3; ++i)
	{
		right.aabb.Add(m_Storage[i]);
	}

	SplitBVHNodeNaive(leftNodeId, level + 1, centroids);
	SplitBVHNodeNaive(rightNodeId, level + 1, centroids);
}

//=================================================================================
bool BVH::Intersect(const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection) const
{
	return IntersectNode(ray, intersection, &m_Nodes[0]);
}

//=================================================================================
bool BVH::IntersectNode(const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection, const BVHNode* node) const
{
	if (node->aabb.IntersectImpl(ray) <= 0.f)
		return false;
	else
	{
		if (node->left != s_InvalidBVHNode || node->right != s_InvalidBVHNode)
		{
			// we can have hit from both sides as sides can overlap
			C_RayIntersection intersections[2];
			bool			  intersectionsResults[2] = {false, false};
			if (node->left != s_InvalidBVHNode)
				intersectionsResults[0] = IntersectNode(ray, intersections[0], &m_Nodes[node->left]);
			if (node->right != s_InvalidBVHNode)
				intersectionsResults[1] = IntersectNode(ray, intersections[1], &m_Nodes[node->right]);
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
	}
	// we are in the leaf node
	struct S_IntersectionInfo {
		C_RayIntersection intersection;
		float			  t;

		[[nodiscard]] bool operator<(const S_IntersectionInfo& a) const { return t < a.t; }
	};
	S_IntersectionInfo closestIntersect{C_RayIntersection(), std::numeric_limits<float>::max()};

	glm::vec2 barycentric;

	for (unsigned int i = node->firstTrig; i <= node->lastTrig; i += 3)
	{
		const glm::vec3* triDef = &(m_Storage[i]);
		const auto		 length = Physics::TraingleRayIntersect(triDef, ray, &barycentric);
		if (length > 0.0f)
		{
			if (closestIntersect.t < length)
			{
				continue;
			}
			auto	   normal = glm::cross(m_Storage[i + 1] - m_Storage[i], m_Storage[i + 2] - m_Storage[i]);
			const auto area	  = glm::length(normal) / 2.f;
			normal			  = glm::normalize(normal);
			C_RayIntersection inter(S_Frame(normal), ray.origin + length * ray.direction, Physics::Primitives::S_Ray(ray));
			inter.SetRayLength(length);

			closestIntersect = {inter, length};
		}
	}
	if (closestIntersect.t == std::numeric_limits<float>::max())
		return false;


	intersection = closestIntersect.intersection;
	return true;
}

//=================================================================================
void BVH::DebugDraw(I_DebugDraw* dd, const glm::mat4& modelMatrix) const
{
	if (m_Nodes.empty())
		return;
	DebugDrawNode(dd, modelMatrix, &m_Nodes[0], 0);
}

//=================================================================================
void BVH::DebugDrawNode(I_DebugDraw* dd, const glm::mat4& modelMatrix, const BVHNode* node, unsigned int level) const
{
	// can be simple loop
	if (!node)
		return;
	static Colours::T_Colour colours[] = {
		Colours::black,
		Colours::green,
		Colours::blue,
		Colours::red,
		Colours::gray,
		Colours::purple,
		Colours::yellow,
		Colours::white,
	};
	constexpr static auto numColours			   = (sizeof(colours) / sizeof(Colours::T_Colour));
	const Colours::T_Colour& currentColour = colours[level < numColours ? level : numColours - 1];
	dd->DrawAABB(node->aabb, currentColour, modelMatrix);
	if (node->left != s_InvalidBVHNode)
		DebugDrawNode(dd, modelMatrix, &m_Nodes[node->left], level + 1);
	if (node->right != s_InvalidBVHNode)
		DebugDrawNode(dd, modelMatrix, &m_Nodes[node->right], level + 1);
}

//=================================================================================
float BVH::CalcSAHCost(const BVHNode& parent, const unsigned int axis, const float splitPos, std::vector<glm::vec3>& centroids) const
{
	Physics::Primitives::S_AABB left, right;
	unsigned int				leftCount = 0, rightCount = 0;
	for (unsigned int i = parent.firstTrig; i < parent.lastTrig + 3; i += 3)
	{
		const glm::vec3* triDef = &(m_Storage[i]);
		if (centroids[i / 3][axis] < splitPos)
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

} // namespace GLEngine::Renderer
