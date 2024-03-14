#include <RendererStdafx.h>

#include <Renderer/DebugDraw.h>
#include <Renderer/RayCasting/Geometry/Trimesh.h>
#include <Renderer/RayCasting/RayIntersection.h>

#include <Physics/GeometryUtils/TriangleIntersect.h>

#include <Utils/HighResolutionTimer.h>
#include <Utils/Range.h>

#include <algorithm>


namespace GLEngine::Renderer {


//=================================================================================
C_Trimesh::~C_Trimesh()
{
	if (m_BVH)
		delete m_BVH;
}

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

	// we check AABB after potentional BVH to avoid double checking
	// we have AABB translated already, so we use original ray
	if (m_Vertices.size() > 3 * 5 && m_AABB.IntersectImpl(rayIn) <= 0.f)
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
		const auto		 length = Physics::TraingleRayIntersect(triDef, ray, &barycentric);
		if (length > 0.0f)
		{
			if (closestIntersect.t < length) {
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
	intersection.SetRayLength(glm::distance(intersection.GetRay().origin, intersection.GetIntersectionPoint()));
	return true;
}

//=================================================================================
void C_Trimesh::AddMesh(const MeshData::Mesh& mesh)
{
	std::transform(mesh.vertices.begin(), mesh.vertices.end(), std::back_inserter(m_Vertices), [](const glm::vec4& i) { return glm::vec3(i); });
	m_AABB = mesh.bbox;
	GLE_ASSERT(m_Vertices.size() % 3 == 0, "Wrong number of vertices.");

	Utils::HighResolutionTimer BVHBuildTime;
	m_BVH = new BVH(m_Vertices);
	CORE_LOG(E_Level::Info, E_Context::Render, "BVH trimesh {} build time {}ms", mesh.m_name, BVHBuildTime.getElapsedTimeFromLastQueryMilliseconds());
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

} // namespace GLEngine::Renderer
