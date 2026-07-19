#pragma once

#include <Renderer/Materials/MaterialResource.h>
#include <Renderer/RayCasting/Geometry/BVH.h>

#include <Physics/Primitives/AABB.h>
#include <Physics/Primitives/Triangle.h>

#include <rttr/registration_friend.h>

namespace GLEngine::Renderer {
class I_DebugDraw;
namespace MeshData {
struct Mesh;
}

class C_Trimesh final {
public:
	C_Trimesh();
	C_Trimesh(const C_Trimesh& other);
	C_Trimesh(C_Trimesh&& other) noexcept;
	C_Trimesh& operator=(const C_Trimesh& other);
	C_Trimesh& operator=(C_Trimesh&& other) noexcept;
	~C_Trimesh();

	[[nodiscard]] bool Intersect(const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection, const float tMax) const;

	void AddTriangle(const Physics::Primitives::S_Triangle& triangle);
	void AddTriangle(const Physics::Primitives::S_Triangle& triangle, const std::array<glm::vec2, 3>& uv);
	void AddMesh(const MeshData::Mesh& mesh);
	void SetBVH(BVH* bvh);

	void										  SetMaterialHandle(const Core::ResourceHandle<MaterialResource>& material);
	const Core::ResourceHandle<MaterialResource>& GetMaterialHandle() const { return m_Material; }

	[[nodiscard]] const Physics::Primitives::S_AABB& GetAABB() const { return m_AABB; }
	[[nodiscard]] Physics::Primitives::S_AABB&		 GetAABB() { return m_AABB; }
	void											 SetTransformation(const glm::mat4& mat);

	[[nodiscard]] std::size_t GetNumTriangles() const;

	void DebugDraw(I_DebugDraw& dd) const;

	RTTR_REGISTRATION_FRIEND

private:
	void			   AfterDeserialize();
	[[nodiscard]] bool IntersectBruteforce(const Physics::Primitives::S_Ray& rayIn, C_RayIntersection& intersection, const float tMax) const;

	std::vector<glm::vec3>				   m_Vertices;
	std::vector<glm::vec2>				   m_TexCoords;
	Physics::Primitives::S_AABB			   m_AABB;
	glm::mat4							   m_Transform	  = glm::mat4(1.f);
	glm::mat4							   m_TransformInv = glm::mat4(1.f);
	BVH*								   m_BVH		  = nullptr;
	Core::ResourceHandle<MaterialResource> m_Material;
	friend class BVH;
	friend class C_TrimeshModel;
};
} // namespace GLEngine::Renderer