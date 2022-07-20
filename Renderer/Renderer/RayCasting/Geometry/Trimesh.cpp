#include <RendererStdafx.h>

#include <Renderer/RayCasting/Geometry/Trimesh.h>
#include <Renderer/RayCasting/RayIntersection.h>

#include <Physics/GeometryUtils/TriangleIntersect.h>

#include <Utils/Range.h>

#include <algorithm>


namespace GLEngine::Renderer {

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
bool C_Trimesh::Intersect(const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection) const
{
	if (m_Vertices.size() > 3 * 5 && m_AABB.IntersectImpl(ray) <= 0.f)
		return false;

	struct S_IntersectionInfo {
		C_RayIntersection					 intersection;
		float								 t;

		[[nodiscard]] bool operator<(const S_IntersectionInfo& a) const { return t < a.t; }
	};
	std::vector<S_IntersectionInfo> intersections;
	intersections.reserve(5);

	glm::vec2 barycentric;

	for (int i = 0; i < m_Vertices.size(); i += 3)
	{
		const glm::vec3* triDef = &(m_Vertices[i]);
		const auto		 length = Physics::TraingleRayIntersect(triDef, ray, &barycentric);
		if (length > 0.0f)
		{
			auto normal = glm::cross(m_Vertices[i + 1] - m_Vertices[i], m_Vertices[i + 2] - m_Vertices[i]);
			const auto area	  = glm::length(normal) / 2.f;
			normal			  = glm::normalize(normal);
			C_RayIntersection inter(S_Frame(normal), ray.origin + length * ray.direction, Physics::Primitives::S_Ray(ray));
			if (!m_TexCoords.empty())
			{
				const glm::vec2* triUV = &(m_TexCoords[i]);
				const glm::vec2	 uv	   = barycentric.x * triUV[0] + barycentric.y * triUV[1] + (1 - barycentric.x - barycentric.y) * triUV[2];
				inter.SetUV(uv);
			}
			inter.SetMaterial(&GetMaterial());

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
void C_Trimesh::AddMesh(const MeshData::Mesh& mesh)
{
	std::transform(mesh.vertices.begin(), mesh.vertices.end(), std::back_inserter(m_Vertices), [](const glm::vec4& i) { return glm::vec3(i); });
	m_AABB = mesh.bbox;
	GLE_ASSERT(m_Vertices.size() % 3 == 0, "Wrong number of vertices.");
}

} // namespace GLEngine::Renderer
