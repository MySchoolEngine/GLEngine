#pragma once

#include <Renderer/IDevice.h>

namespace GLEngine::VkRenderer {
class C_Pipeline {
public:
	C_Pipeline() = default;
	void create(Renderer::I_Device& device, VkFormat swapCahinImageFormat);
	void destroy(Renderer::I_Device& device);

	// todo
	VkRenderPass& GetRenderPass() { return m_RenderPass; }
	VkPipeline&	  GetGraphicsPipeline() { return m_GraphicsPipeline; }

private:
	void CreateRenderPass(Renderer::I_Device& device, VkFormat swapCahinImageFormat);

	VkPipelineLayout m_PipelineLayout;
	VkRenderPass	 m_RenderPass;
	VkPipeline		 m_GraphicsPipeline;
};
}