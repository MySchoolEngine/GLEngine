#pragma once

#include <GLRenderer/Shaders/Generation/ShaderStructs.h>

#include <Renderer/Shaders/CodeGeneration.h>

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

	const C_StructDescriptor GetStructDescription(const std::string& name) const;
private:
	C_ShaderTypesReflection();

	std::map<std::string, C_StructDescriptor> m_StructReflections;
};

class C_GLCodeProvider : public Renderer::Shaders::I_CodeProvider
{
public:
private:
	std::string GetStructCode(const std::string& name) const override;
};

}

#include <GLRenderer/Shaders/Generation/ShaderTypesReflection.inl>
