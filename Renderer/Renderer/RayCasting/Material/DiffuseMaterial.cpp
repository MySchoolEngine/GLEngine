#include <RendererStdAfx.h>

#include <Renderer/Colours.h>
#include <Renderer/RayCasting/Material/DiffuseMaterial.h>
#include <Renderer/RayCasting/RayIntersection.h>
#include <Renderer/RayCasting/ReflectionModels/OrenNayar.h>
#include <Renderer/Textures/TextureStorage.h>
#include <Renderer/Textures/TextureView.h>

namespace GLEngine::Renderer {
//=================================================================================
C_DiffuseMaterial::C_DiffuseMaterial(Colours::T_Colour colour, Core::ResourceHandle<TextureResource> diffuseTexture)
	: m_DiffuseColour(colour) 
	, m_DiffuseTexture(diffuseTexture)
{
}

//=================================================================================
C_DiffuseMaterial::~C_DiffuseMaterial() = default;

//=================================================================================
RayTracingSettings::T_ReflPtr C_DiffuseMaterial::GetScatteringFunction(const C_RayIntersection& intersect, RayTracingSettings::T_ReflAlloc& alloc) const
{
	const auto		  uv			= intersect.GetUV();
	Colours::T_Colour diffuseColour = m_DiffuseColour;
	if (m_DiffuseTexture.IsReady())
	{
		// mutable because I don't have purely read only texture view
		diffuseColour = C_TextureView(const_cast<I_TextureViewStorage*>(&m_DiffuseTexture.GetResource().GetStorage())).Get<Colours::T_Colour, T_Bilinear>(uv);
	}
	return std::unique_ptr<C_OrenNayarModel, RayTracingSettings::T_ReflAlloc::delete_policy<>>(new (alloc.allocate(sizeof(C_OrenNayarModel))) C_OrenNayarModel(diffuseColour, 8.f),
																							   RayTracingSettings::T_ReflAlloc::delete_policy{});
}

} // namespace GLEngine::Renderer
