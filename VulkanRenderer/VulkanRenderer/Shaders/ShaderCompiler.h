#pragma once

#include <Renderer/Shaders/ShaderCompiling.h>

namespace GLEngine::Renderer {
class C_Viewport;
}

namespace GLEngine::VkRenderer {
class C_VkDevice;
class C_ShaderCompiler : public Renderer::ShaderCompilerTrait<VkShaderModule> {
public:
	C_ShaderCompiler(C_VkDevice& device, bool preprocessorOutput = false);
	bool linkProgram(VkPipelineLayout& pipelineLayout, const std::vector<std::pair<Renderer::E_ShaderStage, VkShaderModule>>& stages);
	void ReleaseStage(T_StageHandle& stage) override;
	bool m_PreprocessorOutput;

protected:
	C_VkDevice& m_Device;
	virtual bool
	compileShaderStageInternal(T_StageHandle& stage, const std::filesystem::path& filepath, const Renderer::E_ShaderStage shaderStage, std::vector<char>& content) override;
};
} // namespace GLEngine::VkRenderer
