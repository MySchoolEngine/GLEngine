#pragma once

#include <Renderer/Colours.h>
#include <Renderer/Materials/I_MaterialData.h>
#include <Renderer/Textures/TextureResource.h>

#include <Core/Resources/ResourceHandle.h>

#include <rttr/registration_friend.h>

namespace GLEngine::Renderer {

class RENDERER_API_EXPORT C_PBRMaterialData : public I_MaterialData {
public:
	C_PBRMaterialData() = default;
	explicit C_PBRMaterialData(const std::string& name);

	bool DrawGUI() override;

	const std::string& GetName() const { return m_Name; }

	void SetColor(const Colours::T_Colour& color) { m_Color = color; }
	void SetRoughness(float roughness) { m_Roughness = roughness; }
	void SetColorMapRes(Core::ResourceHandle<TextureResource> handle) { m_ColorMapRes = handle; }
	void SetNormalMapRes(Core::ResourceHandle<TextureResource> handle) { m_NormalMapRes = handle; }
	void SetRoughnessMapRes(Core::ResourceHandle<TextureResource> handle) { m_RoughnessMapRes = handle; }

	RTTR_ENABLE(I_MaterialData)
	RTTR_REGISTRATION_FRIEND

private:
	std::string							  m_Name;
	Colours::T_Colour					  m_Color{Colours::white};
	float								  m_Roughness{0.5f};
	Core::ResourceHandle<TextureResource> m_ColorMapRes;
	Core::ResourceHandle<TextureResource> m_NormalMapRes;
	Core::ResourceHandle<TextureResource> m_RoughnessMapRes;
};

} // namespace GLEngine::Renderer
