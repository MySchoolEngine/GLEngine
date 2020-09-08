#include <GLRendererStdAfx.h>

#include <GLRenderer/Materials/PhongMaterial.h>

#include <GLRenderer/Shaders/Generation/ShaderTypesReflection.h>

#include <GLRenderer/Textures/TextureManager.h>
#include <GLRenderer/Textures/Texture.h>

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
	ret.Push("colorMap",		&C_PhongMaterial::m_ColorMap);
	ret.Push("normalMap",		&C_PhongMaterial::m_NormalMap);
	ret.Push("roughnessMap",	&C_PhongMaterial::m_RoughnessMap);
	ret.Push("modelColor",		&C_PhongMaterial::m_ModelColor);
	ret.Push("roughness",		&C_PhongMaterial::m_Roughness);
	ret.Push("Shininess",		&C_PhongMaterial::m_Shininess);
	ret.Push("useNormalMap",	&C_PhongMaterial::m_UseNormalMap);
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
	const auto identity = Textures::C_TextureManager::Instance().GetIdentityTexture();
	auto* color = static_cast<std::shared_ptr<Textures::C_Texture>*>(material.GetColorMap());
	auto* normalMap = static_cast<std::shared_ptr<Textures::C_Texture>*>(material.GetNormalMap());
	if (color)
	{
		m_ColorMap = (*color)->GetHandle();
		(*color)->MakeHandleResident();
	}
	else
	{
		m_ColorMap = identity->GetHandle();
	}
	m_UseNormalMap = false;
	m_NormalMap = identity->GetHandle();
	if (normalMap)
	{
		m_NormalMap = (*normalMap)->GetHandle();
		(*normalMap)->MakeHandleResident();
		m_UseNormalMap = true;
	}
	m_RoughnessMap = identity->GetHandle();
	m_ModelColor = material.GetColor();
	m_Roughness = material.GetRoughness();
	m_Shininess = material.GetShininess();
}

}
