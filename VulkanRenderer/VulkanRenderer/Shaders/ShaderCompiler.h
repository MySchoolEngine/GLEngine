#pragma once

#include <Renderer/Shaders/ShaderCompiling.h>

namespace GLEngine::VkRenderer::Shaders {

class C_ShaderCompiler : public Renderer::ShaderCompilerTrait<VkPipelineShaderStageCreateInfo> {
public:
	C_ShaderCompiler(VkDevice_T*);
	// bool linkProgram(T_ShaderProgram& program, const std::vector<T_StageHandle>& shaders);

protected:
	virtual bool compileShaderStageInternal(T_StageHandle& stage, const std::filesystem::path& filepath, const Renderer::E_ShaderStage shaderStage, std::string& content) override;


	VkDevice_T* m_Device;
};

} // namespace GLEngine::VkRenderer::Shaders