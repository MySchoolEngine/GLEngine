#pragma once

#include <GLRenderer/GLRendererApi.h>
#include <GLRenderer/Shaders/Generation/ShaderStructs.h>

#include <Renderer/Shaders/CodeGeneration.h>

namespace GLEngine::GLRenderer::Shaders {

class C_ShaderTypesReflection {
public:
	// Singleton stuff
	C_ShaderTypesReflection(C_ShaderTypesReflection const&) = delete;
	void										  operator=(C_ShaderTypesReflection const&) = delete;
	[[nodiscard]] static C_ShaderTypesReflection& Instance();

	template <class T> void Register();

	[[nodiscard]] const C_StructDescriptor GetStructDescription(const std::string& name) const;

private:
	C_ShaderTypesReflection();

	std::map<std::string, C_StructDescriptor> m_StructReflections;
};

class GL_RENDERER_API_EXPORT C_GLCodeProvider final : public Renderer::Shaders::I_CodeProvider {
private:
	[[nodiscard]] std::string GetStructCode(const std::string& name) const override;
};

} // namespace GLEngine::GLRenderer::Shaders

#include <GLRenderer/Shaders/Generation/ShaderTypesReflection.inl>
