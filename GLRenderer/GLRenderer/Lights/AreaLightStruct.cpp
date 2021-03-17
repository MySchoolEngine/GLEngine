#include <GLRendererStdafx.h>

#include <GLRenderer/Lights/AreaLightStruct.h>
#include <GLRenderer/Shaders/Generation/ShaderTypesReflection.h>

namespace GLEngine::GLRenderer {

//=================================================================================
const auto ptlReg = []() {
	Shaders::C_ShaderTypesReflection::Instance().Register<S_AreaLight>();
	return true;
}();

//=================================================================================
S_AreaLight::S_AreaLight()
	: m_LightMat(1.f)
	, m_Pos(0.f)
	, m_Width(10.f)
	, m_Height(10.f)
	, m_Normal(0.f)
	, m_Intensity(1.0f)
	, m_Color(1.f)
{
}

//=================================================================================
Shaders::C_StructDescriptor S_AreaLight::GetDescriptionImpl() const
{
	Shaders::C_StructDescriptor ret("areaLight");
	ret.Push("LightMat", &S_AreaLight::m_LightMat);
	ret.Push("Position", &S_AreaLight::m_Pos);
	ret.Push("Normal", &S_AreaLight::m_Normal);
	ret.Push("Intensity", &S_AreaLight::m_Intensity);
	ret.Push("Color", &S_AreaLight::m_Color);
	ret.Push("ShadowMap", &S_AreaLight::m_ShadowMap);
	ret.Push("Specular", &S_AreaLight::m_Color);
	ret.Push("DirX", &S_AreaLight::m_DirX);
	ret.Push("Width	", &S_AreaLight::m_Width);
	ret.Push("DirY", &S_AreaLight::m_DirY);
	ret.Push("Height", &S_AreaLight::m_Height);
	return ret;
}

//=================================================================================
std::string S_AreaLight::GetNameImpl() const
{
	return "areaLight";
}

} // namespace GLEngine::GLRenderer