#pragma once

#include <Renderer/Colours.h>
#include <Renderer/RendererApi.h>
#include <Renderer/Resources/RenderResourceHandle.h>
#include <Renderer/Textures/DeviceTexture.h>
#include <Renderer/Textures/TextureResource.h>

#include <Core/Resources/ResourceHandle.h>

#include <Utils/RefCounter.h>

#include <rttr/registration.h>
#include <rttr/registration_friend.h>

namespace GLEngine::Renderer::MeshData {
struct Material;
}

namespace GLEngine::Renderer {
class RENDERER_API_EXPORT C_Material : ::Utils::RefCounted<::Utils::SingleThreadCounter> {
public:
	explicit C_Material(const std::string& name);
	explicit C_Material(const MeshData::Material& material);
	C_Material(const C_Material& other) = delete;
	C_Material& operator=(const C_Material& other) = delete;
	C_Material(C_Material&& other) noexcept;

	C_Material Clone() const;
	bool	   IsChanged() const;
	void	   CleanChangeFlag();

	void SetDiffuseColor(const Colours::T_Colour& color);
	void SetRoughness(float roughness);

	void						 SetNormalMap(Handle<Texture> texture);
	void						 SetRoughnessMap(Handle<Texture> texture);
	void						 SetColorMap(Handle<Texture> texture);
	Handle<Texture>				 GetColorMap() const { return m_ColorMap; }
	Handle<Texture>				 GetNormalMap() const { return m_NormalMap; }
	Handle<Texture>				 GetRoughnessMap() const { return m_RoughnessMap; }
	const std::filesystem::path& GetColorMapPath() const;
	const std::filesystem::path& GetNormalMapPath() const;
	const std::filesystem::path& GetRoughnessMapPath() const;
	void						 SetColorMapPath(const std::filesystem::path&);
	void						 SetNormalMapPath(const std::filesystem::path&);
	void						 SetRoughnessMapPath(const std::filesystem::path&);

	const std::string&		 GetName() const { return m_Name; }
	const Colours::T_Colour& GetColor() const { return m_Color; }
	void					 SetColor(const Colours::T_Colour& color) { m_Color = color; }
	float					 GetRoughness() const { return m_Roughness; }

	int	 GetMaterialIndex() const;
	void SetMaterialIndex(int index) { m_MaterialIndex = index; }

	void  SetShininess(float value) { m_Shininess = value; }
	float GetShininess() const { return m_Shininess; }

	bool DrawGUI();

	void Update();

private:
	friend class C_MaterialManager;

	std::string							  m_Name;
	Colours::T_Colour					  m_Color;
	float								  m_Roughness;
	Core::ResourceHandle<TextureResource> m_ColorMapRes;
	Core::ResourceHandle<TextureResource> m_NormalMapRes;
	Core::ResourceHandle<TextureResource> m_RoughnessRes;
	Handle<Texture>						  m_ColorMap;
	Handle<Texture>						  m_NormalMap;
	Handle<Texture>						  m_RoughnessMap;
	mutable bool						  m_Changed : 1; // mutable because of const-ness of DrawGUI
	int									  m_MaterialIndex;
	float								  m_Shininess;
	RTTR_ENABLE();
	RTTR_REGISTRATION_FRIEND;
};
} // namespace GLEngine::Renderer