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
Shaders::C_StructDescriptor S_PointLight::GetDescription() const
{
	Shaders::C_StructDescriptor ret("pointLight");
	ret.Push({ "position", "vec3" });
	ret.Push({ "color", "vec3" });
	ret.Push({ "intensity", "float" });
	return ret;
}

//=================================================================================
std::string S_PointLight::GetName() const
{
	return "pointLight";
}

}
