#include <RendererStdAfx.h>

#include <Renderer/DebugDraw.h>
#include <Renderer/RayCasting/Geometry/BVH.h>

#include <Physics/GeometryUtils/TriangleIntersect.h>

namespace GLEngine::Renderer {

//=================================================================================
BVH::BVH(std::vector<glm::vec3>& storage)
	: m_Storage(storage)
{
	Build();
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
	root.lastTrig  = m_Storage.size() - 3;
	SplitBVHNode(0, 0);
}

//=================================================================================
void BVH::SplitBVHNode(NodeID nodeId, unsigned int level)
{
	if (level > 10)
		return;

	// limit nodes thats too small
	if (m_Nodes[nodeId].lastTrig - m_Nodes[nodeId].firstTrig <= 20 * 3)
		return;

	const auto axis = level % 3;

	// naive split!
	// 1) Allocate two new nodes
	m_Nodes.emplace_back();
	const NodeID leftNodeId = m_Nodes.size() - 1;
	m_Nodes[nodeId].left	= leftNodeId;
	m_Nodes.emplace_back();
	const NodeID rightNodeId = m_Nodes.size() - 1;
	m_Nodes[nodeId].right	 = rightNodeId;
	// now I can store references, because I am not adding new elements
	auto& left	= m_Nodes[leftNodeId];
	auto& right = m_Nodes[rightNodeId];
	auto& node	= m_Nodes[nodeId];

	// first split along X
	const auto	 average	 = node.aabb.m_Min[axis] + (node.aabb.m_Max[axis] - node.aabb.m_Min[axis]) * 0.5f;
	const auto	 trigCenter	 = [&](const glm::vec3* triDef) { return (triDef[0][axis] + triDef[1][axis] + triDef[2][axis]) / 3.f; };
	unsigned int leftSorting = node.firstTrig, rightSorting = node.lastTrig;
	while (leftSorting < rightSorting)
	{
		// find left candidate to swap
		const glm::vec3* triDef = &(m_Storage[leftSorting]);
		if (trigCenter(triDef) < average)
		{
			leftSorting += 3;
			continue;
		}
		// find right candidate for swap
		while (true)
		{
			const glm::vec3* triDef = &(m_Storage[rightSorting]);
			if (trigCenter(triDef) < average)
			{
				break;
			}
			rightSorting -= 3;
		}
		if (leftSorting >= rightSorting)
			break;
		// swap
		std::swap(m_Storage[leftSorting], m_Storage[rightSorting]);
		std::swap(m_Storage[leftSorting + 1], m_Storage[rightSorting + 1]);
		std::swap(m_Storage[leftSorting + 2], m_Storage[rightSorting + 2]);

		leftSorting += 3;
		rightSorting -= 3;
	}

	left.firstTrig	= node.firstTrig;
	left.lastTrig	= leftSorting;
	right.firstTrig = leftSorting + 3; // not using rightSorting to count in all triangles
	right.lastTrig	= node.lastTrig;

	// build AABBs
	for (int i = left.firstTrig; i < left.lastTrig + 3; ++i)
	{
		left.aabb.Add(m_Storage[i]);
	}
	for (int i = right.firstTrig; i < right.lastTrig + 3; ++i)
	{
		right.aabb.Add(m_Storage[i]);
	}

	SplitBVHNode(leftNodeId, level + 1);
	SplitBVHNode(rightNodeId, level + 1);
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
		if (node->left!=-1 || node->right!=-1)
		{
			// we can have hit from both sides as sides can overlap
			C_RayIntersection intersections[2];
			bool			  intersectionsResults[2] = {false, false};
			if (node->left!=-1)
				intersectionsResults[0] = IntersectNode(ray, intersections[0], &m_Nodes[node->left]);
			if (node->right!=-1)
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
	std::vector<S_IntersectionInfo> intersections;
	intersections.reserve(5);

	glm::vec2 barycentric;

	for (int i = node->firstTrig; i <= node->lastTrig; i += 3)
	{
		const glm::vec3* triDef = &(m_Storage[i]);
		const auto		 length = Physics::TraingleRayIntersect(triDef, ray, &barycentric);
		if (length > 0.0f)
		{
			auto	   normal = glm::cross(m_Storage[i + 1] - m_Storage[i], m_Storage[i + 2] - m_Storage[i]);
			const auto area	  = glm::length(normal) / 2.f;
			normal			  = glm::normalize(normal);
			C_RayIntersection inter(S_Frame(normal), ray.origin + length * ray.direction, Physics::Primitives::S_Ray(ray));

			intersections.push_back({inter, length});
		}
	}

	std::sort(intersections.begin(), intersections.end());

	if (intersections.empty())
		return false;


	intersection = intersections[0].intersection;
	return true;
}

//=================================================================================
void BVH::DebugDraw(I_DebugDraw* dd, const glm::mat4& modelMatrix) const
{
	if (m_Nodes.empty())
		return;
	DebugDrawNode(dd, modelMatrix, &m_Nodes[0]);
}

//=================================================================================
void BVH::DebugDrawNode(I_DebugDraw* dd, const glm::mat4& modelMatrix, const BVHNode* node) const
{
	// can be simple loop
	if (!node)
		return;
	dd->DrawAABB(node->aabb, Colours::yellow, modelMatrix);
	if (node->left != -1)
		DebugDrawNode(dd, modelMatrix, &m_Nodes[node->left]);
	if (node->right != -1)
		DebugDrawNode(dd, modelMatrix, &m_Nodes[node->right]);
}

} // namespace GLEngine::Renderer
