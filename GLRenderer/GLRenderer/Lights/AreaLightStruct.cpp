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
	, m_Radius(10.f)	
	, m_Normal(0.f)
	, m_Intensity(1.0f)
	, m_Color(1.f) {}

//=================================================================================
Shaders::C_StructDescriptor S_AreaLight::GetDescriptionImpl() const
{
	Shaders::C_StructDescriptor ret("areaLight");
	ret.Push("LightMat",	&S_AreaLight::m_LightMat);
	ret.Push("Position",	&S_AreaLight::m_Pos);
	ret.Push("Radius",		&S_AreaLight::m_Radius);
	ret.Push("Normal",		&S_AreaLight::m_Normal);
	ret.Push("Intensity", &S_AreaLight::m_Intensity);
	ret.Push("Color",			&S_AreaLight::m_Color);
	ret.Push("ShadowMap",			&S_AreaLight::m_ShadowMap);
	ret.Push("DirX",			&S_AreaLight::m_DirX);
	ret.Push("DirY",			&S_AreaLight::m_DirY);
	return ret;
}

//=================================================================================
std::string S_AreaLight::GetNameImpl() const
{
	return "areaLight";
}

}