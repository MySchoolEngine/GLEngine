#include <GLRendererStdafx.h>

#include <GLRenderer/Shaders/Generation/ShaderTypesReflection.h>

namespace GLEngine::GLRenderer::Shaders {

//=================================================================================
C_ShaderTypesReflection& C_ShaderTypesReflection::Instance()
{
	static C_ShaderTypesReflection    instance; // Guaranteed to be destroyed.
							// Instantiated on first use.
	return instance;
}

//=================================================================================
C_ShaderTypesReflection::C_ShaderTypesReflection()
{

}

//=================================================================================
const C_StructDescriptor C_ShaderTypesReflection::GetStructDescription(const std::string& name) const
{
	const auto it = m_StructReflections.find(name);
	if (it == m_StructReflections.end())
	{
		return C_StructDescriptor("EMPTY");
	}

	return it->second;
}

}
