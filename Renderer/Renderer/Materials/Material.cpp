#include <RendererStdafx.h>

#include <Renderer/Materials/Material.h>
#include <Renderer/Mesh/Scene.h>

#include <imgui.h>

namespace GLEngine::Renderer {

//=================================================================================
C_Material::C_Material(const std::string& name)
	: m_Name(name)
	, m_Color(1.f)
	, m_Roughness(0.5f)
	, m_ColorMap(nullptr)
	, m_NormalMap(nullptr)
	, m_RoughnessMap(nullptr)
	, m_Changed(true)
	, m_MaterialIndex(-1)
	, m_Shininess(32.f)
{
}

//=================================================================================
C_Material::C_Material(const MeshData::Material& material)
	: m_Name(material.m_Name)
	, m_Color(1.f)
	, m_Roughness(0.5f)
	, m_ColorMap(nullptr)
	, m_NormalMap(nullptr)
	, m_RoughnessMap(nullptr)
	, m_Changed(true)
	, m_MaterialIndex(-1)
	, m_Shininess(material.shininess)
{
}

//=================================================================================
C_Material::C_Material(C_Material&& other)
	: m_Name(std::move(other.m_Name))
	, m_Color(other.m_Color)
	, m_Roughness(other.m_Roughness)
	, m_ColorMap(other.m_ColorMap)
	, m_NormalMap(other.m_NormalMap)
	, m_RoughnessMap(other.m_RoughnessMap)
	, m_Changed(other.m_Changed)
	, m_MaterialIndex(other.m_MaterialIndex)
	, m_Shininess(other.GetShininess())
{
}

//=================================================================================
C_Material C_Material::clone() const
{
	return C_Material("");
}

//=================================================================================
bool C_Material::IsChanged() const
{
	return m_Changed;
}

//=================================================================================
void C_Material::CleanChangeFlag()
{
	m_Changed = false;
}

//=================================================================================
void C_Material::SetDiffuseColor(const glm::vec3& color)
{
	m_Color = color;
}

//=================================================================================
void C_Material::SetRoughness(float roughness)
{
	m_Roughness = roughness;
}

//=================================================================================
void C_Material::SetNormalMap(std::shared_ptr<I_DeviceTexture> texture)
{
	m_NormalMap = texture;
}

//=================================================================================
void C_Material::SetRoughnessMap(std::shared_ptr<I_DeviceTexture> texture)
{
	m_Roughness	   = 1.0f;
	m_RoughnessMap = texture;
}

//=================================================================================
void C_Material::SetColorMap(std::shared_ptr<I_DeviceTexture> texture)
{
	m_ColorMap = texture;
	m_Color	   = glm::vec3(1.0f, 1.0f, 1.0f);
}

//=================================================================================
int C_Material::GetMaterialIndex() const
{
	return m_MaterialIndex;
}

//=================================================================================
void C_Material::DrawGUI() const
{
	if (auto normalMap = GetNormalMap())
		ImGui::Image((void*)(intptr_t)(normalMap->GetDeviceTextureHandle()), ImVec2(256, 256));
	if (auto roughnessMap = GetRoughnessMap())
		ImGui::Image((void*)(intptr_t)(roughnessMap->GetDeviceTextureHandle()), ImVec2(256, 256));
	if (auto colourMap = GetColorMap())
		ImGui::Image((void*)(intptr_t)(colourMap->GetDeviceTextureHandle()), ImVec2(256, 256));
}

} // namespace GLEngine::Renderer
