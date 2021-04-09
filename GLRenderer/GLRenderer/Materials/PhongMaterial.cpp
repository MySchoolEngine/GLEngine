#include <GLRendererStdafx.h>

#include <GLRenderer/Materials/PhongMaterial.h>
#include <GLRenderer/Shaders/Generation/ShaderTypesReflection.h>
#include <GLRenderer/Textures/Texture.h>
#include <GLRenderer/Textures/TextureManager.h>

namespace GLEngine::GLRenderer::Material {

//=================================================================================
const auto ptlReg = []() {
	Shaders::C_ShaderTypesReflection::Instance().Register<C_PhongMaterial>();
	return true;
}();

//=================================================================================
Shaders::C_StructDescriptor C_PhongMaterial::GetDescriptionImpl() const
{
	Shaders::C_StructDescriptor ret("PhongMaterial");
	ret.Push("colorMap", &C_PhongMaterial::m_ColorMap);
	ret.Push("normalMap", &C_PhongMaterial::m_NormalMap);
	ret.Push("roughnessMap", &C_PhongMaterial::m_RoughnessMap);
	ret.Push("modelColor", &C_PhongMaterial::m_ModelColor);
	ret.Push("roughness", &C_PhongMaterial::m_Roughness);
	ret.Push("Shininess", &C_PhongMaterial::m_Shininess);
	ret.Push("useNormalMap", &C_PhongMaterial::m_UseNormalMap);
	return ret;
}

//=================================================================================
std::string C_PhongMaterial::GetNameImpl() const
{
	return "PhongMaterial";
}

//=================================================================================
void C_PhongMaterial::Update(const Renderer::C_Material& material)
{
	const auto identity		= Textures::C_TextureManager::Instance().GetIdentityTexture();
	auto	   color		= std::dynamic_pointer_cast<Textures::C_Texture>(material.GetColorMap());
	auto	   normalMap	= std::dynamic_pointer_cast<Textures::C_Texture>(material.GetNormalMap());
	auto	   roughnessMap = std::dynamic_pointer_cast<Textures::C_Texture>(material.GetRoughnessMap());
	if (color)
	{
		m_ColorMap = color->CreateHandle();
		color->MakeHandleResident();
	}
	else
	{
		m_ColorMap = identity->CreateHandle();
	}
	m_UseNormalMap = false;
	m_NormalMap	   = identity->GetHandle();
	if (normalMap)
	{
		m_NormalMap = normalMap->CreateHandle();
		normalMap->MakeHandleResident();
		m_UseNormalMap = true;
	}
	m_RoughnessMap = identity->GetHandle();
	if (roughnessMap)
	{
		m_RoughnessMap = roughnessMap->CreateHandle();
		roughnessMap->MakeHandleResident();
	}
	m_ModelColor = material.GetColor();
	m_Roughness	 = material.GetRoughness();
	m_Shininess	 = material.GetShininess();
}

} // namespace GLEngine::GLRenderer::Material
