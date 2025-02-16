#include <RendererStdafx.h>

#include <Renderer/DebugDraw.h>
#include <Renderer/Mesh/Scene.h>
#include <Renderer/RayCasting/Geometry/Trimesh.h>
#include <Renderer/RayCasting/RayIntersection.h>

#include <Physics/GeometryUtils/TriangleIntersect.h>

#include <Utils/Range.h>
#include <Utils/Serialization/SerializationUtils.h>

#include <algorithm>
#include <rttr/registration>

// clang-format off
RTTR_REGISTRATION
{
	using namespace GLEngine::Renderer;
	rttr::registration::class_<C_Trimesh>("Trimesh")
		.constructor<>()(rttr::policy::ctor::as_object)
		.property("Vertices", &C_Trimesh::m_Vertices)(rttr::policy::prop::as_reference_wrapper)
		.property("Transofrm", &C_Trimesh::m_Transofrm)(rttr::policy::prop::as_reference_wrapper, REGISTER_DEFAULT_VALUE(glm::mat4(1.f)))
		.property("AABB", &C_Trimesh::m_AABB);
}
// clang-format on

namespace GLEngine::Renderer {
//=================================================================================
C_Trimesh::C_Trimesh() = default;

//=================================================================================
C_Trimesh::~C_Trimesh() = default;

//=================================================================================
void C_Trimesh::AddTriangle(const Physics::Primitives::S_Triangle& triangle)
{
	GLE_ASSERT(m_Vertices.size() % 3 == 0, "Wrong number of vertices.");
	m_Vertices.push_back(triangle.m_p[0]);
	m_Vertices.push_back(triangle.m_p[1]);
	m_Vertices.push_back(triangle.m_p[2]);
	m_AABB.Add(triangle.m_p[0]);
	m_AABB.Add(triangle.m_p[1]);
	m_AABB.Add(triangle.m_p[2]);
}

//=================================================================================
void C_Trimesh::AddTriangle(const Physics::Primitives::S_Triangle& triangle, const std::array<glm::vec2, 3>& uv)
{
	GLE_ASSERT(m_Vertices.size() == m_TexCoords.size(), "Mix matching uv and uv-less triangles.");
	AddTriangle(triangle);
	m_TexCoords.push_back(uv[0]);
	m_TexCoords.push_back(uv[1]);
	m_TexCoords.push_back(uv[2]);
}

//=================================================================================
bool C_Trimesh::Intersect(const Physics::Primitives::S_Ray& rayIn, C_RayIntersection& intersection) const
{
	const auto ray = Physics::Primitives::S_Ray{m_TransofrmInv * glm::vec4(rayIn.origin, 1.f), rayIn.direction};

	if (m_BVH)
	{
		if (m_BVH->Intersect(ray, intersection))
		{
			intersection.SetMaterial(&GetMaterial());
			intersection.TransformRayAndPoint(m_Transofrm);
			intersection.SetRayLength(glm::distance(intersection.GetRay().origin, intersection.GetIntersectionPoint()));
			return true;
		}
		return false;
	}

	if (m_AABB.IntersectImpl(rayIn) <= 0.f)
		return false;

	struct S_IntersectionInfo {
		C_RayIntersection intersection;
		float			  t;

		[[nodiscard]] bool operator<(const S_IntersectionInfo& a) const { return t < a.t; }
	};

	S_IntersectionInfo closestIntersect{C_RayIntersection(), std::numeric_limits<float>::max()};

	glm::vec2 barycentric;

	for (int i = 0; i < m_Vertices.size(); i += 3)
	{
		const glm::vec3* triDef = &(m_Vertices[i]);
		const auto		 length = Physics::TriangleRayIntersect(triDef, ray, &barycentric);
		if (length > 0.0f)
		{
			if (closestIntersect.t < length)
			{
				continue;
			}
			auto	   normal = glm::cross(m_Vertices[i + 1] - m_Vertices[i], m_Vertices[i + 2] - m_Vertices[i]);
			const auto area	  = glm::length(normal) / 2.f;
			normal			  = glm::normalize(normal);
			C_RayIntersection inter(S_Frame(normal), ray.origin + length * ray.direction, Physics::Primitives::S_Ray(ray));
			if (!m_TexCoords.empty())
			{
				const glm::vec2* triUV = &(m_TexCoords[i]);
				const glm::vec2	 uv	   = barycentric.x * triUV[1] + barycentric.y * triUV[2] + (1 - barycentric.x - barycentric.y) * triUV[0];
				inter.SetUV(uv);
			}
			inter.SetMaterial(&GetMaterial());

			closestIntersect = {inter, length};
		}
	}

	if (closestIntersect.t == std::numeric_limits<float>::max())
		return false;


	intersection = closestIntersect.intersection;
	intersection.TransformRayAndPoint(m_Transofrm);
	intersection.SetRayLength(glm::distance(intersection.GetRay().origin, intersection.GetIntersectionPoint())); // todo tady mohu pou��t length ne?
	return true;
}

//=================================================================================
void C_Trimesh::AddMesh(const MeshData::Mesh& mesh)
{
	m_Vertices.reserve(mesh.vertices.size());
	m_Vertices = mesh.vertices;
	m_AABB	   = mesh.bbox;
	GLE_ASSERT(m_Vertices.size() % 3 == 0, "Wrong number of vertices.");
}

//=================================================================================
void C_Trimesh::SetTransformation(glm::mat4 mat)
{
	auto invMat	   = glm::inverse(mat);
	m_AABB		   = m_AABB.getTransformedAABB(mat);
	m_Transofrm	   = mat;
	m_TransofrmInv = invMat;
}

//=================================================================================
void C_Trimesh::DebugDraw(I_DebugDraw* dd) const
{
	if (m_BVH)
		m_BVH->DebugDraw(dd, m_Transofrm);
}

//=================================================================================
std::size_t C_Trimesh::GetNumTriangles() const
{
	return m_Vertices.size() / 3;
}

//=================================================================================
void C_Trimesh::SetBVH(const BVH* bvh)
{
	m_BVH = bvh;
}

} // namespace GLEngine::Renderer
