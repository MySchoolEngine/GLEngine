#include <RendererStdafx.h>

#include <Renderer/Materials/Material.h>
#include <Renderer/Mesh/Scene.h>

#include <imgui.h>

namespace GLEngine::Renderer {

//=================================================================================
C_Material::C_Material(const std::string& name)
	: m_Name(name)
	, m_Color("Color", Colours::white)
	, m_Roughness(0.5f, 0.f, 1.f, "Roughness")
	, m_ColorMap(nullptr)
	, m_NormalMap(nullptr)
	, m_RoughnessMap(nullptr)
	, m_Changed(true)
	, m_MaterialIndex(-1)
	, m_Shininess(32.f)
	, m_Textures({GUI::C_Texture{m_ColorMap}, GUI::C_Texture{m_NormalMap}, GUI::C_Texture{m_RoughnessMap}})
{
	SetTextureCB();
}

//=================================================================================
C_Material::C_Material(const MeshData::Material& material)
	: m_Name(material.m_Name)
	, m_Color("Color", Colours::white)
	, m_Roughness(0.5f, 0.f, 1.f, "Roughness")
	, m_ColorMap(nullptr)
	, m_NormalMap(nullptr)
	, m_RoughnessMap(nullptr)
	, m_Changed(true)
	, m_MaterialIndex(-1)
	, m_Shininess(material.shininess)
	, m_Textures({GUI::C_Texture{m_ColorMap}, GUI::C_Texture{m_NormalMap}, GUI::C_Texture{m_RoughnessMap}})
{
	SetTextureCB();
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
	, m_Textures({GUI::C_Texture{m_ColorMap}, GUI::C_Texture{m_NormalMap}, GUI::C_Texture{m_RoughnessMap}})
{
	SetTextureCB();
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
void C_Material::SetDiffuseColor(const Colours::T_Colour& color)
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
	m_NormalMap	  = texture;
	m_Textures[1] = GUI::C_Texture(m_NormalMap);
	SetTextureCB();
	m_Changed = true;
}

//=================================================================================
void C_Material::SetRoughnessMap(std::shared_ptr<I_DeviceTexture> texture)
{
	m_Roughness	   = 1.0f;
	m_RoughnessMap = texture;
	m_Textures[2]  = GUI::C_Texture(m_RoughnessMap);
	SetTextureCB();
	m_Changed = true;
}

//=================================================================================
void C_Material::SetColorMap(std::shared_ptr<I_DeviceTexture> texture)
{
	m_ColorMap	  = texture;
	m_Color		  = Colours::white;
	m_Textures[0] = GUI::C_Texture(m_ColorMap);
	SetTextureCB();
	m_Changed = true;
}

//=================================================================================
int C_Material::GetMaterialIndex() const
{
	return m_MaterialIndex;
}

//=================================================================================
void C_Material::DrawGUI() const
{
	m_Color.Draw();
	m_Roughness.Draw();
	if (m_Roughness.Changed())
		m_Changed = true;
	for (const auto& it : m_Textures)
		it.Draw();
}

//=================================================================================
void C_Material::SetTextureCB()
{
	m_Textures[0].SetOnTextureCleanCB([&]() { SetColorMap(nullptr); });
	m_Textures[1].SetOnTextureCleanCB([&]() { SetNormalMap(nullptr); });
	m_Textures[2].SetOnTextureCleanCB([&]() { SetRoughnessMap(nullptr); });
}

} // namespace GLEngine::Renderer
