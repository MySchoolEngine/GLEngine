#pragma once

#include <Renderer/RayCasting/Geometry/SceneGeometry.h>

#include <Physics/Primitives/AABB.h>

namespace GLEngine::Renderer {
class C_GeometryList : public I_RayGeometryObject {
public:
	C_GeometryList() = default;

	[[nodiscard]] virtual bool Intersect(const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection) const override;
	[[nodiscard]] virtual float Area() const override { return 0.f; }

	void AddObject(std::shared_ptr<I_RayGeometryObject>&& object);

	[[nodiscard]] const Physics::Primitives::S_AABB& GetAABB() const { return m_AABB; }
	[[nodiscard]] Physics::Primitives::S_AABB& GetAABB() { return m_AABB; }
private:
	std::vector<std::shared_ptr<I_RayGeometryObject>> m_Geometry;
	Physics::Primitives::S_AABB						  m_AABB;
};
} // namespace GLEngine::Renderer