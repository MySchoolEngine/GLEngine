#pragma once

#include <GLRenderer/Shaders/Generation/ShaderStructs.h>

namespace GLEngine::GLRenderer::Shaders {

class C_ShaderTypesReflection
{
public:
	//Singleton stuff
	C_ShaderTypesReflection(C_ShaderTypesReflection const&) = delete;
	void operator=(C_ShaderTypesReflection const&) = delete;
	static C_ShaderTypesReflection& Instance();

	template<class T>
	void Register();

	const C_StructDescriptor GetStructDescription(const std::string name) const;
private:
	C_ShaderTypesReflection();

	std::map<std::string, C_StructDescriptor> m_StructReflections;
};

}

#include <GLRenderer/Shaders/Generation/ShaderTypesReflection.inl>
