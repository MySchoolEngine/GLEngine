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
	const Colours::T_Colour& GetColour() const { return m_Color; }
	void SetRoughness(float roughness) { m_Roughness = roughness; }
	float					 GetRoughness() const { return m_Roughness; }
	void SetColorMapRes(const Core::ResourceHandle<TextureResource>& handle) { m_ColorMapRes = handle; }
	const Core::ResourceHandle<TextureResource>& GetColorMapRes() const { return m_ColorMapRes; }
	void										 SetNormalMapRes(const Core::ResourceHandle<TextureResource>& handle) { m_NormalMapRes = handle; }
	const Core::ResourceHandle<TextureResource>& GetNormalMapRes() const { return m_NormalMapRes; }
	void SetRoughnessMapRes(const Core::ResourceHandle<TextureResource>& handle) { m_RoughnessMapRes = handle; }
	const Core::ResourceHandle<TextureResource>& GetRoughnessMapRes() const { return m_RoughnessMapRes; }

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
