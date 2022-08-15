#pragma once

namespace GLEngine::GLRenderer::Shaders {

//=================================================================================
template <class T> void GLEngine::GLRenderer::Shaders::C_ShaderTypesReflection::Register()
{
	static_assert(sizeof(T) % (sizeof(float) * 4) == 0, "Buffer must be aligned to four floats for GPU");
	T obj;
	if constexpr (std::is_base_of_v<I_GLSLStruct<T>, T>)
	{
		m_StructReflections.emplace(obj.GetName(), obj.GetDescription());
	}
	else
	{
		static_assert(!std::is_base_of_v<I_GLSLStruct<T>, T>, "T is not valid GLSL struct");
	}
}
} // namespace GLEngine::GLRenderer::Shaders
