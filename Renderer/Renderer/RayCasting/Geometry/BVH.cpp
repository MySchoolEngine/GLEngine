#include <RendererStdAfx.h>

#include <Renderer/DebugDraw.h>
#include <Renderer/RayCasting/Geometry/BVH.h>

#include <Physics/GeometryUtils/TriangleIntersect.h>

namespace GLEngine::Renderer {

//=================================================================================
BVH::BVH(std::vector<glm::vec3>& storage)
	: m_Storage(storage)
	, m_Root(nullptr)
{
	Build();
}

//=================================================================================
BVH::~BVH()
{
	DeleteNode(m_Root);
}

//=================================================================================
void BVH::Build()
{
	if (m_Storage.empty())
		return;

	m_Root = new BVHNode();
	for (const auto& vertex : m_Storage)
		m_Root->aabb.Add(vertex);
	m_Root->firstTrig = 0;
	m_Root->lastTrig  = m_Storage.size() - 3;
	SplitBVHNode(m_Root, 0);
}

//=================================================================================
void BVH::SplitBVHNode(BVHNode* node, unsigned int level)
{
	if (level > 10)
		return;

	// limit nodes thats too small
	if (node->lastTrig - node->firstTrig <= 20 * 3)
		return;

	const auto axis = level % 3;

	// naive split!
	// 1) Allocate two new nodes
	auto left	= new BVHNode();
	auto right	= new BVHNode();
	node->left	= left;
	node->right = right;
	// first split along X
	const auto	 average	 = node->aabb.m_Min[axis] + (node->aabb.m_Max[axis] - node->aabb.m_Min[axis]) * 0.5f;
	const auto	 trigCenter	 = [&](const glm::vec3* triDef) { return (triDef[0][axis] + triDef[1][axis] + triDef[2][axis]) / 3.f; };
	unsigned int leftSorting = node->firstTrig, rightSorting = node->lastTrig;
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

	node->left->firstTrig  = node->firstTrig;
	node->left->lastTrig   = leftSorting;
	node->right->firstTrig = leftSorting + 3; // not using rightSorting to count in all triangles
	node->right->lastTrig  = node->lastTrig;

	// build AABBs
	for (int i = node->left->firstTrig; i < node->left->lastTrig + 3; ++i)
	{
		node->left->aabb.Add(m_Storage[i]);
	}
	for (int i = node->right->firstTrig; i < node->right->lastTrig + 3; ++i)
	{
		node->right->aabb.Add(m_Storage[i]);
	}

	SplitBVHNode(node->left, level + 1);
	SplitBVHNode(node->right, level + 1);
}

//=================================================================================
bool BVH::Intersect(const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection) const
{
	return IntersectNode(ray, intersection, m_Root);
}

//=================================================================================
bool BVH::IntersectNode(const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection, const BVHNode* node) const
{
	if (node->aabb.IntersectImpl(ray) <= 0.f)
		return false;
	else
	{
		if (node->left || node->right)
		{
			// we can have hit from both sides as sides can overlap
			C_RayIntersection intersections[2];
			bool			  intersectionsResults[2] = {false, false};
			if (node->left)
				intersectionsResults[0] = IntersectNode(ray, intersections[0], node->left);
			if (node->right)
				intersectionsResults[1] = IntersectNode(ray, intersections[1], node->right);
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
	DebugDrawNode(dd, modelMatrix, m_Root);
}

//=================================================================================
void BVH::DebugDrawNode(I_DebugDraw* dd, const glm::mat4& modelMatrix, const BVHNode* node) const
{
	if (!node)
		return;
	dd->DrawAABB(node->aabb, Colours::yellow, modelMatrix);
	DebugDrawNode(dd, modelMatrix, node->left);
	DebugDrawNode(dd, modelMatrix, node->right);
}

//=================================================================================
void BVH::DeleteNode(BVHNode* node)
{
	if (node)
	{
		DeleteNode(node->left);
		DeleteNode(node->right);
		delete node;
	}
}

} // namespace GLEngine::Renderer
