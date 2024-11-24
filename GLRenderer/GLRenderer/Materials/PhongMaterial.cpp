#include <GLRendererStdafx.h>

#include <GLRenderer/Materials/PhongMaterial.h>
#include <GLRenderer/OGLRenderer.h>
#include <GLRenderer/Shaders/Generation/ShaderTypesReflection.h>
#include <GLRenderer/Textures/Texture.h>
#include <GLRenderer/Textures/TextureManager.h>

#include <Renderer/IRenderer.h>
#include <Renderer/Resources/ResourceManager.h>

#include <Core/Application.h>

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
bool C_PhongMaterial::Update(const Renderer::C_Material& material)
{
	bool	   result			  = true;
	auto&	   gRM				  = static_cast<C_OGLRenderer&>(Core::C_Application::Get().GetActiveRenderer()).GetRMGL();
	const auto identity			  = Textures::C_TextureManager::Instance().GetIdentityTexture();
	auto	   colorHandle		  = material.GetColorMap();
	auto	   normalMapHandle	  = material.GetNormalMap();
	auto	   roughnessMapHandle = material.GetRoughnessMap();

	auto color		  = gRM.GetTexture(colorHandle);
	auto normalMap	  = gRM.GetTexture(normalMapHandle);
	auto roughnessMap = gRM.GetTexture(roughnessMapHandle);
	if (color)
	{
		result &= color->IsPresentOnGPU();
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
		result &= normalMap->IsPresentOnGPU();
		m_NormalMap = normalMap->CreateHandle();
		normalMap->MakeHandleResident();
		m_UseNormalMap = normalMap->IsPresentOnGPU();
	}
	m_RoughnessMap = identity->GetHandle();
	if (roughnessMap)
	{
		result &= roughnessMap->IsPresentOnGPU();
		m_RoughnessMap = roughnessMap->CreateHandle();
		roughnessMap->MakeHandleResident();
	}
	m_ModelColor = material.GetColor();
	m_Roughness	 = material.GetRoughness();
	m_Shininess	 = material.GetShininess();

	return result;
}

} // namespace GLEngine::GLRenderer::Material
