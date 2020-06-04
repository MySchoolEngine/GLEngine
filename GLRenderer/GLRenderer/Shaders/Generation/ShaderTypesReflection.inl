#pragma once

namespace GLEngine::GLRenderer::Shaders {

//=================================================================================
template<class T>
void GLEngine::GLRenderer::Shaders::C_ShaderTypesReflection::Register()
{
	T obj;
	if constexpr (std::is_base_of_v<I_GLSLStruct<T>, T>)
	{
		m_StructReflections.emplace(obj.GetName(), obj.GetDescription());
	}
	else
	{
		static_assert(false, "T is not valid GLSL struct");
	}
}
}
