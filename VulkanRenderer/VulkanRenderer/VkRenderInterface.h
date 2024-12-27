#pragma once

#include <Renderer/Renderer3D.h>

namespace GLEngine::VkRenderer {
class C_VkRenderer;
class C_Pipeline;
class C_VkRenderInterface : public Renderer::I_RenderInterface3D
{
public:
	C_VkRenderInterface();
	~C_VkRenderInterface();
	void BeginRender(VkCommandBuffer commandBuffer, VkDescriptorSet descriptorSet, C_Pipeline* pipeline);
	void Render(const Renderer::RenderCall3D& call) override;
	void EndRender();

private:
	VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;
	VkDescriptorSet m_DescriptorSet = VK_NULL_HANDLE;
	C_Pipeline*		m_Pipeline		= nullptr;
};
} // namespace GLEngine::VkRenderer
