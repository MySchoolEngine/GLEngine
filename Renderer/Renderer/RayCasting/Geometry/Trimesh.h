#pragma once

#include <Renderer/RayCasting/Geometry/BVH.h>
#include <Renderer/RayCasting/Geometry/SceneGeometry.h>

#include <Physics/Primitives/AABB.h>
#include <Physics/Primitives/Triangle.h>

#include <rttr/registration_friend.h>

namespace GLEngine::Renderer {
class I_DebugDraw;
namespace MeshData {
struct Mesh;
}

class C_Trimesh : public I_RayGeometryObject {
public:
	C_Trimesh(const C_Trimesh& other);
	C_Trimesh(C_Trimesh&& other) noexcept;
	C_Trimesh& operator=(const C_Trimesh& other);
	C_Trimesh& operator=(C_Trimesh&& other) noexcept;
	C_Trimesh();
	~C_Trimesh() override;
	[[nodiscard]] bool Intersect(const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection) const override;

	float Area() const override { return 0.0f; }

	void AddTriangle(const Physics::Primitives::S_Triangle& triangle);
	void AddTriangle(const Physics::Primitives::S_Triangle& triangle, const std::array<glm::vec2, 3>& uv);
	void AddMesh(const MeshData::Mesh& mesh);
	void SetBVH(const BVH* bvh);

	[[nodiscard]] const Physics::Primitives::S_AABB& GetAABB() const { return m_AABB; }
	[[nodiscard]] Physics::Primitives::S_AABB&		 GetAABB() { return m_AABB; }

	// only scale and translate
	void SetTransformation(glm::mat4 mat);

	std::size_t GetNumTriangles() const;

	void DebugDraw(I_DebugDraw& dd) const;

	RTTR_ENABLE();
	RTTR_REGISTRATION_FRIEND;

private:
	std::vector<glm::vec3>		m_Vertices;
	std::vector<glm::vec2>		m_TexCoords;
	Physics::Primitives::S_AABB m_AABB;
	glm::mat4					m_Transofrm	   = glm::mat4(1.f);
	glm::mat4					m_TransofrmInv = glm::mat4(1.f);
	const BVH*					m_BVH		   = nullptr;
	friend class BVH;
	friend class C_TrimeshModel;
};
} // namespace GLEngine::Renderer