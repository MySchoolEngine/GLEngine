#pragma once

#include <Renderer/Colours.h>
#include <Renderer/RendererApi.h>
#include <Renderer/Textures/DeviceTexture.h>

#include <Utils/RefCounter.h>

namespace GLEngine::Renderer::MeshData {
struct Material;
}

namespace GLEngine::Renderer {
class RENDERER_API_EXPORT C_Material : Utils::RefCounted<Utils::SingleThreadCounter> {
public:
	C_Material(const std::string& name);
	C_Material(const MeshData::Material& material);
	C_Material(const C_Material& other) = delete;
	C_Material& operator=(const C_Material& other) = delete;
	C_Material(C_Material&& other);

	C_Material clone() const;
	bool	   IsChanged() const;
	void	   CleanChangeFlag();

	void SetDiffuseColor(const Colours::T_Colour& color);
	void SetRoughness(float roughness);

	void							 SetNormalMap(std::shared_ptr<I_DeviceTexture> texture);
	void							 SetRoughnessMap(std::shared_ptr<I_DeviceTexture> texture);
	void							 SetColorMap(std::shared_ptr<I_DeviceTexture> texture);
	std::shared_ptr<I_DeviceTexture> GetColorMap() const { return m_ColorMap; }
	std::shared_ptr<I_DeviceTexture> GetNormalMap() const { return m_NormalMap; }
	std::shared_ptr<I_DeviceTexture> GetRoughnessMap() const { return m_RoughnessMap; }

	const std::string&		 GetName() const { return m_Name; }
	const Colours::T_Colour& GetColor() const { return m_Color; }
	void					 SetColor(const Colours::T_Colour& color) { m_Color = color; }
	float					 GetRoughness() const { return m_Roughness; }

	int	 GetMaterialIndex() const;
	void SetMaterialIndex(int index) { m_MaterialIndex = index; }

	void  SetShininess(float value) { m_Shininess = value; }
	float GetShininess() const { return m_Shininess; }

	void DrawGUI() const;

private:
	std::string						 m_Name;
	Colours::T_Colour				 m_Color;
	float							 m_Roughness;
	std::shared_ptr<I_DeviceTexture> m_ColorMap;
	std::shared_ptr<I_DeviceTexture> m_NormalMap;
	std::shared_ptr<I_DeviceTexture> m_RoughnessMap;
	bool							 m_Changed : 1;
	int								 m_MaterialIndex;
	float							 m_Shininess;
};
} // namespace GLEngine::Renderer