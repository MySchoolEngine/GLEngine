#pragma once

#include <Renderer/RayCasting/Geometry/SceneGeometry.h>
#include <Renderer/RayCasting/Geometry/TrimeshModel.h>
#include <Renderer/RayCasting/Material/MaterialProviderInterface.h>


namespace GLEngine::Renderer {
class C_TrimeshModel;
class C_StaticRTMesh : public I_RayGeometryObject {
public:
	explicit C_StaticRTMesh(const Core::ResourceHandle<C_TrimeshModel>& model);
	~C_StaticRTMesh() override;
	[[nodiscard]] bool Intersect(const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection, const float tMax) const override;

	[[nodiscard]] float Area() const override;
	void				SetTransformation(const glm::mat4& mat);

	void InitMaterials(I_MaterialProviderInterface& materialProvider);

private:
	glm::mat4										   m_Transform	  = glm::mat4(1.f);
	glm::mat4										   m_InvTransform = glm::mat4(1.f);
	Core::ResourceHandle<C_TrimeshModel>			   m_TrimeshModel;
	Physics::Primitives::S_SSEAABB					   m_AABB;
	std::vector<I_MaterialInterface*>				   m_Materials;
	std::vector<Core::ResourceHandle<TextureResource>> m_AlphaMaps;
	// trimesh, maybe transformed AABB?

	friend class StaticRTMeshFixture;		 // test-only access to populate m_Materials/m_AlphaMaps without the real InitMaterials() pipeline
	friend class RayTraceSceneAlphaFixture; // test-only access to populate m_Materials/m_AlphaMaps without the real InitMaterials() pipeline
};
} // namespace GLEngine::Renderer