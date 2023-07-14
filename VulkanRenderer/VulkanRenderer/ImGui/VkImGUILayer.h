#pragma once

#include <GUI/ImGuiLayer.h>

namespace GLEngine::VkRenderer {
struct VkGuiInit {
	VkInstance		 Instance;
	VkPhysicalDevice PhysicalDevice;
	VkDevice		 Device;
	uint32_t		 QueueFamily;
	VkQueue			 Queue;
	VkPipelineCache	 PipelineCache;
	uint32_t		 MinImageCount;
	uint32_t		 ImageCount;
	VkRenderPass	 RenderPass;
	VkCommandBuffer	 CommandBuffer; //< used to init fonts
};
class C_VkImGUILayer : public GUI::C_ImGuiLayer {
public:
	C_VkImGUILayer(GUID window);
	void		 Init(const VkGuiInit& inti);
	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void FrameBegin() override;
	void		 SetCommandBuffer(VkCommandBuffer CommandBuffer);
	virtual void FrameEnd(Core::I_Input& input) override;

private:
	void			CreateDescriptorPool(const VkGuiInit& init);
	VkCommandBuffer m_CommandBuffer;
	VkDescriptorPool DescriptorPool;
};
} // namespace GLEngine::VkRenderer
