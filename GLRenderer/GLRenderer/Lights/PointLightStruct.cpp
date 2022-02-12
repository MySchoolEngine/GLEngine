#include <GLRendererStdafx.h>

#include <GLRenderer/Lights/PointLightStruct.h>
#include <GLRenderer/Shaders/Generation/ShaderTypesReflection.h>


namespace GLEngine::GLRenderer {

//=================================================================================
const auto ptlReg = []() {
	Shaders::C_ShaderTypesReflection::Instance().Register<S_PointLight>();
	return true;
}();

//=================================================================================
S_PointLight::S_PointLight()
	: m_Position(0.f)
	, m_Color(Colours::black)
	, m_Intensity(0.f)
{
}

//=================================================================================
Shaders::C_StructDescriptor S_PointLight::GetDescriptionImpl() const
{
	Shaders::C_StructDescriptor ret("pointLight");
	ret.Push("position", &S_PointLight::m_Position);
	ret.Push("color", &S_PointLight::m_Color);
	ret.Push("intensity", &S_PointLight::m_Intensity);
	return ret;
}

//=================================================================================
std::string S_PointLight::GetNameImpl() const
{
	return "pointLight";
}

} // namespace GLEngine::GLRenderer
