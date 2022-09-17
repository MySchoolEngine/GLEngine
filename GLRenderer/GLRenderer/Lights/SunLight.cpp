#include <GLRendererStdafx.h>

#include <GLRenderer/Lights/SunLight.h>
#include <GLRenderer/Shaders/Generation/ShaderTypesReflection.h>

namespace GLEngine::GLRenderer {

//=================================================================================
const auto slReg = []() {
	Shaders::C_ShaderTypesReflection::Instance().Register<S_SunLight>();
	return true;
}();

//=================================================================================
S_SunLight::S_SunLight()
	: m_SunPosition(0, 1, 0)
	, m_SunDiscMultiplier(1.f)
	, m_SunColor(Colours::white)
	, m_AsymetricFactor(0.6f)
	, m_SunShadowMap(0)
	, m_SunlightPresent(1)
{
}

//=================================================================================
Shaders::C_StructDescriptor S_SunLight::GetDescriptionImpl() const
{
	Shaders::C_StructDescriptor ret("sunLight");
	ret.Push("position", &S_SunLight::m_SunPosition);
	ret.Push("discMultiplier", &S_SunLight::m_SunDiscMultiplier);
	ret.Push("color", &S_SunLight::m_SunColor);
	ret.Push("asymetricFactor", &S_SunLight::m_AsymetricFactor);
	ret.Push("viewProjection", &S_SunLight::m_LightViewProjection);
	ret.Push("sunlightPresent", &S_SunLight::m_SunlightPresent);
	ret.Push("sunShadowMap", &S_SunLight::m_SunShadowMap);
	return ret;
}

//=================================================================================
std::string S_SunLight::GetNameImpl() const
{
	return "sunLight";
}

} // namespace GLEngine::GLRenderer
