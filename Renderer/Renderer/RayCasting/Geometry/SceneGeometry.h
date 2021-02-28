#pragma once

#include <Renderer/Mesh/Scene.h>
#include <Renderer/RayCasting/RayIntersection.h>

namespace GLEngine::Physics::Primitives {
struct S_Ray;
}

namespace GLEngine::Renderer {

class I_RayGeometryObject {
public:
	virtual ~I_RayGeometryObject()																						= default;
	[[nodiscard]] virtual bool	Intersect(const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection) const = 0;
	[[nodiscard]] virtual float Area() const																			= 0;
	[[nodiscard]] float			Pdf() const { return 1.f / Area(); }

	void					  SetMaterial(const MeshData::Material& material) { m_Material = material; }
	const MeshData::Material& GetMaterial() const { return m_Material; }

private:
	MeshData::Material m_Material;
};
} // namespace GLEngine::Renderer