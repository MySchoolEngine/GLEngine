#pragma once

#include <Renderer/RayCasting/Material/MaterialInterface.h>
#include <Renderer/Textures/TextureResource.h>
#include <Renderer/Colours.h>

#include <Core/Resources/ResourceHandle.h>

namespace GLEngine::Renderer {
class C_DiffuseMaterial : public I_MaterialInterface {
public:
	C_DiffuseMaterial(Colours::T_Colour colour, Core::ResourceHandle<TextureResource> diffuseTexture = {});
	virtual ~C_DiffuseMaterial();

	[[nodiscard]] virtual RayTracingSettings::T_ReflPtr GetScatteringFunction(const C_RayIntersection& intersect, RayTracingSettings::T_ReflAlloc& alloc) const override;

private:
	Colours::T_Colour					  m_DiffuseColour;
	Core::ResourceHandle<TextureResource> m_DiffuseTexture;
};

} // namespace GLEngine::Renderer
