#pragma once

#include <Renderer/Descriptors/PipelineDescriptor.h>
#include <Renderer/IDevice.h>

namespace GLEngine::VkRenderer {
class C_Pipeline {
public:
	C_Pipeline() = default;
	void create(Renderer::I_Device& device, Renderer::PipelineDescriptor desc, VkFormat swapCahinImageFormat);
	void destroy(Renderer::I_Device& device);

	// todo
	VkRenderPass& GetRenderPass() { return m_RenderPass; }
	VkPipeline&	  GetGraphicsPipeline() { return m_GraphicsPipeline; }
	VkDescriptorSetLayout& GetDescriptorSetLayout() { return m_DescriptorSetLayout; }
	VkPipelineLayout&	   GetLayout() { return m_PipelineLayout; }

private:
	void CreateRenderPass(Renderer::I_Device& device, VkFormat swapCahinImageFormat);

	
	VkDescriptorSetLayout		 m_DescriptorSetLayout;
	VkPipelineLayout			 m_PipelineLayout;
	VkRenderPass				 m_RenderPass;
	VkPipeline					 m_GraphicsPipeline;
	Renderer::PipelineDescriptor m_Desc;
};
} // namespace GLEngine::VkRenderer