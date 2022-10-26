#include <RendererStdafx.h>

#include <Renderer/Materials/Material.h>
#include <Renderer/Mesh/Scene.h>

#include <GLRenderer/Textures/TextureManager.h>

#include <Core/Resources/ResourceManager.h>

#include <imgui.h>
#include <rttr/registration>

RTTR_REGISTRATION
{
	using namespace GLEngine::Renderer;

	rttr::registration::class_<C_Material>("C_Material")
		.constructor<std::string>()
		.property("Name", &C_Material::m_Name)
		.property("Color", &C_Material::m_Color)
		.property_readonly("Roughness", &C_Material::GetRoughness)
		.property("ColorMap", &C_Material::GetColorMapPath, &C_Material::SetColorMapPath)
		.property("NormalMap", &C_Material::GetNormalMapPath, &C_Material::SetNormalMapPath)
		.property("RoughnessMap", &C_Material::GetRoughnessMapPath, &C_Material::SetRoughnessMapPath)
		.property("Shininess", &C_Material::GetShininess, &C_Material::SetShininess);

	rttr::type::register_wrapper_converter_for_base_classes<std::shared_ptr<C_Material>>();
}

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
	, m_Color("Color", material.diffuse)
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
void C_Material::SetNormalMapPath(const std::filesystem::path& path)
{
	auto& rm	   = Core::C_ResourceManager::Instance();
	m_NormalMapRes = rm.LoadResource<TextureResource>(path);
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
void C_Material::SetRoughnessMapPath(const std::filesystem::path& path)
{
	auto& rm	   = Core::C_ResourceManager::Instance();
	m_RoughnessRes = rm.LoadResource<TextureResource>(path);
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
void C_Material::SetColorMapPath(const std::filesystem::path& path)
{
	auto& rm	  = Core::C_ResourceManager::Instance();
	m_ColorMapRes = rm.LoadResource<TextureResource>(path);
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

//=================================================================================
const std::filesystem::path& C_Material::GetColorMapPath() const
{
	if (m_ColorMapRes)
		return m_ColorMapRes.GetResource().GetFilepath();
	return {};
}

//=================================================================================
const std::filesystem::path& C_Material::GetNormalMapPath() const
{
	if (m_NormalMapRes)
		return m_NormalMapRes.GetResource().GetFilepath();
	return {};
}

//=================================================================================
const std::filesystem::path& C_Material::GetRoughnessMapPath() const
{
	if (m_RoughnessRes)
		return m_RoughnessRes.GetResource().GetFilepath();
	return {};
}

} // namespace GLEngine::Renderer
