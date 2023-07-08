#include <VulkanRendererStdafx.h>

#include <VulkanRenderer/Pipeline.h>
#include <VulkanRenderer/VkRenderInterface.h>
#include <VulkanRenderer/VkRenderer.h>

#include <Core/Application.h>

namespace GLEngine::VkRenderer {

//=================================================================================
C_VkRenderInterface::C_VkRenderInterface() = default;

//=================================================================================
C_VkRenderInterface::~C_VkRenderInterface() = default;

//=================================================================================
void C_VkRenderInterface::Render(const Renderer::RenderCall3D& call)
{
	GLE_ASSERT(m_CommandBuffer != VK_NULL_HANDLE, "Wrong command buffer");
	GLE_ASSERT(m_DescriptorSet != VK_NULL_HANDLE, "Wrong command buffer");
	GLE_ASSERT(m_Pipeline != nullptr, "Wrong command buffer");

	auto& renderer = Core::C_Application::Get().GetActiveRenderer();
	auto& vkRM	   = static_cast<C_VkRenderer&>(renderer).GetRMVK();

	C_VkBuffer* pPosBuffer		= vkRM.GetBuffer(call.Buffers[0]);
	C_VkBuffer* pNorBuffer		= vkRM.GetBuffer(call.Buffers[1]);
	C_VkBuffer* pTexCoordBuffer = vkRM.GetBuffer(call.Buffers[2]);

	GLE_ASSERT(pPosBuffer && pNorBuffer && pTexCoordBuffer, "Buffers not created");

	VkBuffer	 vertexBuffers[] = {pPosBuffer->GetBuffer(), pNorBuffer->GetBuffer(), pTexCoordBuffer->GetBuffer()};
	VkDeviceSize offsets[]		 = {0, 0, 0};
	vkCmdBindVertexBuffers(m_CommandBuffer, 0, 3, vertexBuffers, offsets);
	// vkCmdBindIndexBuffer(m_CommandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT16);
	vkCmdBindDescriptorSets(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline->GetLayout(), 0, 1, &m_DescriptorSet, 0, nullptr);
	// vkCmdDrawIndexed(m_CommandBuffer, 6u, 1, 0, 0, 0);
	vkCmdDraw(m_CommandBuffer, static_cast<GLsizei>(960), 1, 0, 0);
}

//=================================================================================
void C_VkRenderInterface::BeginRender(VkCommandBuffer commandBuffer, VkDescriptorSet descriptorSet, C_Pipeline* pipeline)
{
	m_CommandBuffer = commandBuffer;
	m_DescriptorSet = descriptorSet;
	m_Pipeline		= pipeline;
}

//=================================================================================
void C_VkRenderInterface::EndRender()
{
	m_CommandBuffer = VK_NULL_HANDLE;
	m_DescriptorSet = VK_NULL_HANDLE;
}

} // namespace GLEngine::VkRenderer
