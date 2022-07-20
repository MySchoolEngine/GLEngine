#pragma once

#include <Renderer/RayCasting/Geometry/SceneGeometry.h>

#include <Physics/Primitives/AABB.h>
#include <Physics/Primitives/Triangle.h>

namespace GLEngine::Renderer {

class C_Trimesh : public I_RayGeometryObject
{
public:
	[[nodiscard]] virtual bool	Intersect(const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection) const override;

	float Area() const override
	{
		return 0.0f;
	}

	void AddTriangle(const Physics::Primitives::S_Triangle& triangle);
	void AddTriangle(const Physics::Primitives::S_Triangle& triangle, const std::array<glm::vec2, 3>& uv);
	void AddMesh(const MeshData::Mesh& mesh);

	[[nodiscard]] const Physics::Primitives::S_AABB& GetAABB() const { return m_AABB; }
	[[nodiscard]] Physics::Primitives::S_AABB& GetAABB() { return m_AABB; }

private:
	std::vector<glm::vec3>		m_Vertices;
	std::vector<glm::vec2>		m_TexCoords;
	Physics::Primitives::S_AABB m_AABB;
};
}