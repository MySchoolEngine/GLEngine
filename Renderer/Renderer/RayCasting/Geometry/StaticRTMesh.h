#pragma once

#include <Renderer/RayCasting/Geometry/SceneGeometry.h>
#include <Renderer/RayCasting/Geometry/TrimeshModel.h>


namespace GLEngine::Renderer {
class C_TrimeshModel;
class C_StaticRTMesh : public I_RayGeometryObject {
public:
	C_StaticRTMesh(const Core::ResourceHandle<C_TrimeshModel>& model);
	~C_StaticRTMesh() override;
	[[nodiscard]] bool Intersect(const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection, const float tMax) const override;

	[[nodiscard]] float Area() const override;
	void				SetTransformation(const glm::mat4& mat);

private:
	glm::mat4							 m_Transform	= glm::mat4(1.f);
	glm::mat4							 m_InvTransform = glm::mat4(1.f);
	Core::ResourceHandle<C_TrimeshModel> m_TrimeshModel;
	Physics::Primitives::S_SSEAABB		 m_AABB;
	// trimesh, maybe transformed AABB?
};
} // namespace GLEngine::Renderer