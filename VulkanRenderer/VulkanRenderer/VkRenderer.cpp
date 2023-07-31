#include <VulkanRendererStdafx.h>

#include <VulkanRenderer/VkBuffer.h>
#include <VulkanRenderer/VkRenderer.h>
#include <VulkanRenderer/VkTypeHelpers.h>

#include <Renderer/IRenderBatch.h>
#include <Renderer/IRenderCommand.h>
#include <Renderer/Resources/ResourceManager.h>
#include <Renderer/Textures/TextureStorage.h>

namespace GLEngine::VkRenderer {

//=================================================================================
C_VkRenderer::C_VkRenderer(VkInstance instance, VkSurfaceKHR surface)
	: m_CommandQueue(new std::remove_pointer<decltype(m_CommandQueue)>::type)
	, m_Instance(instance)
{
	InitDevice(surface);
	m_GPUResourceManager.Init(&m_Device);
	InitDefaultCommandPool();
}

//=================================================================================
C_VkRenderer::~C_VkRenderer()
{
	vkDestroyCommandPool(GetDeviceVK(), m_DefaultCommandPool, nullptr);
	vkDestroyDevice(m_VkDevice, nullptr);
	delete m_CommandQueue;
}

//=================================================================================
void C_VkRenderer::Lock(bool lock /*= true*/)
{
	m_Locked = lock;
}

//=================================================================================
void C_VkRenderer::AddCommand(T_CommandPtr command)
{
	if (m_Locked)
	{
		__debugbreak();
	}
	m_CommandQueue->emplace_back(std::move(command));
}

//=================================================================================
void C_VkRenderer::Commit() const
{
	for (auto& command : (*m_CommandQueue))
	{
		command->Commit();
	}
}

//=================================================================================
void C_VkRenderer::ClearCommandBuffers()
{
	m_CommandQueue->clear();
}

//=================================================================================
bool C_VkRenderer::InitDevice(VkSurfaceKHR surface)
{
	auto pfnCreateDevice = (PFN_vkCreateDevice)glfwGetInstanceProcAddress(m_Instance, "vkCreateDevice");

	uint32_t gpuCount = 0;
	vkEnumeratePhysicalDevices(m_Instance, &gpuCount, nullptr);
	GLE_ASSERT(gpuCount > 0, "No GPU detected");
	std::vector<VkPhysicalDevice> gpus(gpuCount);
	vkEnumeratePhysicalDevices(m_Instance, &gpuCount, gpus.data());
	VkPhysicalDevice_T* gpu = gpus[0];

	{
		uint32_t familyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(gpu, &familyCount, nullptr);
		std::vector<VkQueueFamilyProperties> familyProperties(familyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(gpu, &familyCount, familyProperties.data());

		uint32_t i	   = 0;
		bool	 found = false;
		for (auto it : familyProperties)
		{
			if (it.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				m_GraphicsFamilyIndex = i;
				found				  = true;
			}
			else if (it.queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
				m_ComputeFamilyIndex = i;
			}
			else if (it.queueFlags & VK_QUEUE_TRANSFER_BIT)
			{
				m_TransferFamilyIndex = i;
			}

			VkBool32   presentSupport = false;
			const auto ret			  = vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, surface, &presentSupport);
			if (ret == VK_SUCCESS && presentSupport)
			{
				m_PresentingFamilyIndex = i;
			}
			++i;
		}
		if (!found)
		{
			GLE_ASSERT(found, "Vulkan: Not found queue supporting graphics.");
			std::exit(EXIT_FAILURE);
		}
	}

	const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

	const auto queueInfos = CreatePresentingQueueInfos();

	VkPhysicalDeviceFeatures device_features{};
	device_features.samplerAnisotropy = VK_TRUE;

	VkDeviceCreateInfo device_create_info{};
	device_create_info.sType				   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_create_info.pNext				   = nullptr;
	device_create_info.queueCreateInfoCount	   = static_cast<uint32_t>(queueInfos.size());
	device_create_info.pQueueCreateInfos	   = queueInfos.data();
	device_create_info.pEnabledFeatures		   = &device_features;
	device_create_info.enabledExtensionCount   = static_cast<uint32_t>(deviceExtensions.size());
	device_create_info.ppEnabledExtensionNames = deviceExtensions.data();


	auto ret = pfnCreateDevice(gpu, &device_create_info, nullptr, &m_VkDevice);
	if (ret != VK_SUCCESS)
	{
		CORE_LOG(E_Level::Error, E_Context::Core, "Vulkan device unable to initialize. Error code: '{}'", ret);
		std::exit(EXIT_FAILURE);
	}

	vkGetDeviceQueue(m_VkDevice, m_GraphicsFamilyIndex, 0, &m_graphicsQueue);
	vkGetDeviceQueue(m_VkDevice, m_TransferFamilyIndex, 0, &m_TransferQueue);
	vkGetDeviceQueue(m_VkDevice, m_PresentingFamilyIndex, 0, &m_presentQueue);

	m_Device.Init(m_VkDevice, gpu);

	return true;
}

//=================================================================================
bool C_VkRenderer::InitDefaultCommandPool()
{
	const VkCommandPoolCreateInfo poolInfo{
		.sType			  = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.flags			  = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = GetGraphicsFamilyIndex(),
	};

	if (const auto result = vkCreateCommandPool(GetDeviceVK(), &poolInfo, nullptr, &m_DefaultCommandPool) != VK_SUCCESS)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Failed to create command pool. {}", result);
		return false;
	}
	return true;
}

//=================================================================================
std::vector<VkDeviceQueueCreateInfo> C_VkRenderer::CreatePresentingQueueInfos()
{
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t>					 uniqueQueueFamilies = {m_GraphicsFamilyIndex, m_TransferFamilyIndex, m_PresentingFamilyIndex};

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType			 = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount		 = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;


		queueCreateInfos.push_back(queueCreateInfo);
	}

	return queueCreateInfos;
}

//=================================================================================
Renderer::E_PassType C_VkRenderer::GetCurrentPassType() const
{
	return Renderer::E_PassType::FinalPass;
}

//=================================================================================
void C_VkRenderer::FillFamilyIndexes(VkSwapchainCreateInfoKHR& createInfo)
{
	uint32_t queueFamilyIndices[] = {m_GraphicsFamilyIndex, m_PresentingFamilyIndex};

	if (m_GraphicsFamilyIndex != m_PresentingFamilyIndex)
	{
		createInfo.imageSharingMode		 = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices	 = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode		 = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;		// Optional
		createInfo.pQueueFamilyIndices	 = nullptr; // Optional
	}
}

//=================================================================================
void C_VkRenderer::AddTransferCommand(T_CommandPtr)
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
Renderer::I_Device& C_VkRenderer::GetDevice()
{
	return m_Device;
}

//=================================================================================
uint32_t C_VkRenderer::GetGraphicsFamilyIndex() const
{
	return m_GraphicsFamilyIndex;
}

//=================================================================================
VkQueue C_VkRenderer::GetGraphicsQueue() const
{
	return m_graphicsQueue;
}

//=================================================================================
VkQueue C_VkRenderer::GetPresentationQueue() const
{
	return m_presentQueue;
}

//=================================================================================
void C_VkRenderer::CopyBuffer(VkBuffer srcBuffer, Renderer::Handle<Renderer::Buffer> dstBuffer, VkDeviceSize size, VkCommandPool& commandPool)
{
	VkCommandBuffer commandBuffer = BeginSingleTimeCommands(commandPool);

	const VkBufferCopy copyRegion{
		.srcOffset = 0, // Optional
		.dstOffset = 0, // Optional
		.size	   = size,
	};

	C_VkBuffer* pdstBuffer = m_GPUResourceManager.GetBuffer(dstBuffer);
	GLE_ASSERT(pdstBuffer, "buffer not found");

	vkCmdCopyBuffer(commandBuffer, srcBuffer, pdstBuffer->GetBuffer(), 1, &copyRegion);

	EndSigneTimeCommands(commandBuffer, commandPool);
}

//=================================================================================
void C_VkRenderer::SetTextureData(Renderer::Handle<Renderer::Texture> dstTexture, const Renderer::I_TextureViewStorage& storage)
{
	const glm::uvec2   dim		 = storage.GetDimensions();
	const VkDeviceSize imageSize = dim.x * dim.y * 4 * sizeof(float); // todo, could be different type

	VkBuffer	   stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	m_Device.CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
						  stagingBufferMemory);

	void* data;
	vkMapMemory(GetDeviceVK(), stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, storage.GetData(), static_cast<size_t>(imageSize));
	vkUnmapMemory(GetDeviceVK(), stagingBufferMemory);

	C_VkTexture* pdstTexture = m_GPUResourceManager.GetTexture(dstTexture);

	// formats should be deduced from desc
	TransitionImageLayout(pdstTexture->textureImage, GetTextureFormat(pdstTexture->m_Desc.format), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_DefaultCommandPool);
	CopyBufferToImage(stagingBuffer, pdstTexture->textureImage, static_cast<uint32_t>(dim.x), static_cast<uint32_t>(dim.y), m_DefaultCommandPool);
	TransitionImageLayout(pdstTexture->textureImage, GetTextureFormat(pdstTexture->m_Desc.format), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
						  m_DefaultCommandPool);

	vkDestroyBuffer(GetDeviceVK(), stagingBuffer, nullptr);
	vkFreeMemory(GetDeviceVK(), stagingBufferMemory, nullptr);
}

//=================================================================================
void C_VkRenderer::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkCommandPool& commandPool)
{
	VkCommandBuffer commandBuffer = BeginSingleTimeCommands(commandPool);

	VkImageMemoryBarrier barrier{
		.sType				  = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.srcAccessMask = 0, // TODO
		.dstAccessMask = 0, // TODO
		.oldLayout			  = oldLayout,
		.newLayout			  = newLayout,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image				  = image,
		.subresourceRange	  = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel	= 0,
			.levelCount		= 1,
			.baseArrayLayer = 0,
			.layerCount		= 1,
		},
	};

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage		 = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage		 = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
	{
		throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

	EndSigneTimeCommands(commandBuffer, commandPool);
}

//=================================================================================
void C_VkRenderer::CopyBufferToImage(VkBuffer srcBuffer, VkImage image, uint32_t width, uint32_t height, VkCommandPool& commandPool)
{
	VkCommandBuffer commandBuffer = BeginSingleTimeCommands(commandPool);

	VkBufferImageCopy region{
		.bufferOffset		 = 0,
		.bufferRowLength	 = 0,
		.bufferImageHeight = 0,
		.imageSubresource = {
			.aspectMask	   = VK_IMAGE_ASPECT_COLOR_BIT,
			.mipLevel	   = 0,
			.baseArrayLayer = 0,
			.layerCount	   = 1,
		},
		.imageOffset = {0, 0, 0},
		.imageExtent = {width, height, 1},
	};

	vkCmdCopyBufferToImage(commandBuffer, srcBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	EndSigneTimeCommands(commandBuffer, commandPool);
}

//=================================================================================
VkCommandBuffer C_VkRenderer::BeginSingleTimeCommands(VkCommandPool& commandPool)
{
	const VkCommandBufferAllocateInfo allocInfo{
		.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool		= commandPool,
		.level				= VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1,
	};

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(GetDeviceVK(), &allocInfo, &commandBuffer);

	const VkCommandBufferBeginInfo beginInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
	};

	vkBeginCommandBuffer(commandBuffer, &beginInfo);
	return commandBuffer;
}

//=================================================================================
void C_VkRenderer::EndSigneTimeCommands(VkCommandBuffer& commandBuffer, VkCommandPool& commandPool)
{
	vkEndCommandBuffer(commandBuffer);

	const VkSubmitInfo submitInfo{
		.sType				= VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = 1,
		.pCommandBuffers	= &commandBuffer,
	};

	vkQueueSubmit(GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(GetGraphicsQueue());

	vkFreeCommandBuffers(GetDeviceVK(), commandPool, 1, &commandBuffer);
}

//=================================================================================
VkQueue C_VkRenderer::GetTransferQueue() const
{
	return m_TransferQueue;
}

//=================================================================================
void C_VkRenderer::SetBufferData(Renderer::Handle<Renderer::Buffer> dstBuffer, std::size_t numBytes, const void* data)
{
	auto* buffer = m_GPUResourceManager.GetBuffer(dstBuffer);
	if (buffer->GetDesc().usage == Renderer::E_ResourceUsage::Persistent) {
		buffer->UploadData(data, numBytes);
		return;
	}
	VkBuffer	   stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	m_Device.CreateBuffer(numBytes, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
							   stagingBufferMemory);

	void* dataDst;
	vkMapMemory(GetDeviceVK(), stagingBufferMemory, 0, numBytes, 0, &dataDst);
	memcpy(dataDst, data, (size_t)numBytes);
	vkUnmapMemory(GetDeviceVK(), stagingBufferMemory);

	CopyBuffer(stagingBuffer, dstBuffer, numBytes, m_DefaultCommandPool);

	vkDestroyBuffer(GetDeviceVK(), stagingBuffer, nullptr);
	vkFreeMemory(GetDeviceVK(), stagingBufferMemory, nullptr);
}

//=================================================================================
Renderer::ResouceManager& C_VkRenderer::GetRM()
{
	return m_GPUResourceManager;
}

//=================================================================================
C_VkResourceManager& C_VkRenderer::GetRMVK()
{
	return m_GPUResourceManager;
}
//=================================================================================
void* C_VkRenderer::GetTextureGPUHandle(Renderer::Handle<Renderer::Texture> textureHanlde)
{
	if (auto* texture = m_GPUResourceManager.GetTexture(textureHanlde))
	{
		return texture->GetView();
	}
	return VK_NULL_HANDLE;
}

} // namespace GLEngine::VkRenderer

