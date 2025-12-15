#pragma once

#include <Renderer/RayCasting/Material/MaterialInterface.h>
#include <Renderer/Textures/TextureResource.h>

namespace GLEngine::Physics::Primitives {
struct S_Ray;
}

namespace GLEngine::Renderer {

class C_RayIntersection;

class I_RayGeometryObject {
public:
	virtual ~I_RayGeometryObject()																						= default;
	[[nodiscard]] virtual bool	Intersect(const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection) const = 0;
	[[nodiscard]] virtual float Area() const																			= 0;
	[[nodiscard]] float			Pdf() const { return 1.f / Area(); }

	void				 SetMaterial(I_MaterialInterface* material) { m_MaterialInter = material; }
	I_MaterialInterface& GetMaterial() const
	{
		GLE_ASSERT(m_MaterialInter, "Using uninitialized scene!");
		return *m_MaterialInter;
	}

	void SetAlphaMask(Core::ResourceHandle<TextureResource> alphaMask) { m_AlphaMask = alphaMask; }

protected:
	Core::ResourceHandle<TextureResource> m_AlphaMask;

private:
	I_MaterialInterface*				  m_MaterialInter = nullptr;
};
} // namespace GLEngine::Renderer