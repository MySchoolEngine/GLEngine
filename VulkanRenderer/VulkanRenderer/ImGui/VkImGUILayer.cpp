#include <VulkanRendererStdafx.h>

#include <VulkanRenderer/ImGui/VkImGUILayer.h>

#include <Core/Input.h>

#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>

namespace GLEngine::VkRenderer {

//=================================================================================
C_VkImGUILayer::C_VkImGUILayer(GUID window)
	: GUI::C_ImGuiLayer(window)
{
}

//=================================================================================
void C_VkImGUILayer::OnAttach()
{
	GUI::C_ImGuiLayer::OnAttach();
}

//=================================================================================
void C_VkImGUILayer::Init(const VkGuiInit& init)
{
	CreateDescriptorPool(init);

	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance					= init.Instance;
	init_info.PhysicalDevice			= init.PhysicalDevice;
	init_info.Device					= init.Device;
	init_info.QueueFamily				= init.QueueFamily;
	init_info.Queue						= init.Queue;
	init_info.PipelineCache				= init.PipelineCache;
	init_info.DescriptorPool			= DescriptorPool;
	init_info.MinImageCount				= init.MinImageCount;
	init_info.ImageCount				= init.ImageCount;
	// init_info.Allocator					= inti.All;
	//init_info.CheckVkResultFn			= check_vk_result;
	ImGui_ImplVulkan_Init(&init_info, init.RenderPass);


	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(init.CommandBuffer, &begin_info);

	ImGui_ImplVulkan_CreateFontsTexture(init.CommandBuffer);

	VkSubmitInfo end_info = {
		.sType				= VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = 1,
		.pCommandBuffers	= &init.CommandBuffer,
	};
	vkEndCommandBuffer(init.CommandBuffer);
	vkQueueSubmit(init.Queue, 1, &end_info, VK_NULL_HANDLE);

	vkDeviceWaitIdle(init.Device);
	ImGui_ImplVulkan_DestroyFontUploadObjects();
}

//=================================================================================
void C_VkImGUILayer::OnDetach()
{
	ImGui_ImplVulkan_Shutdown();
	GUI::C_ImGuiLayer::OnDetach();
}

//=================================================================================
void C_VkImGUILayer::FrameBegin()
{
	::ImGuiIO& io = ::ImGui::GetIO();

	float time	 = (float)glfwGetTime();
	io.DeltaTime = m_Time > 0.0f ? (time - m_Time) : (1.0f / 60.0f);
	m_Time		 = time;

	ImGui_ImplVulkan_NewFrame();

	GUI::C_ImGuiLayer::FrameBegin();
}

//=================================================================================
void C_VkImGUILayer::FrameEnd(Core::I_Input& input)
{
	GUI::C_ImGuiLayer::FrameEnd(input);
	ImGui_ImplVulkan_RenderDrawData(GetRenderData(), m_CommandBuffer);
}

//=================================================================================
void C_VkImGUILayer::SetCommandBuffer(VkCommandBuffer CommandBuffer)
{
	m_CommandBuffer = CommandBuffer;
}

//=================================================================================
void C_VkImGUILayer::CreateDescriptorPool(const VkGuiInit& init)
{
	constexpr int			   size			= 200;
	VkDescriptorPoolSize	   pool_sizes[] = {{VK_DESCRIPTOR_TYPE_SAMPLER, size},
										   {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, size},
										   {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, size},
										   {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, size},
										   {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, size},
										   {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, size},
										   {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, size},
										   {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, size},
										   {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, size},
										   {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, size},
										   {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, size}};
	VkDescriptorPoolCreateInfo pool_info	= {};
	pool_info.sType							= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags							= VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets						= size * IM_ARRAYSIZE(pool_sizes);
	pool_info.poolSizeCount					= (uint32_t)IM_ARRAYSIZE(pool_sizes);
	pool_info.pPoolSizes					= pool_sizes;
	if (const auto result = vkCreateDescriptorPool(init.Device, &pool_info, nullptr, &DescriptorPool) != VK_SUCCESS)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "failed to create descriptor pool. {}", result);
		return;
	}
}

} // namespace GLEngine::VkRenderer