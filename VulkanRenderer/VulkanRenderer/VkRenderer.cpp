#include <VulkanRendererStdafx.h>

#include <VulkanRenderer/VkRenderer.h>

#include <Renderer/IRenderBatch.h>
#include <Renderer/IRenderCommand.h>

namespace GLEngine::VkRenderer {

//=================================================================================
C_VkRenderer::C_VkRenderer(VkInstance instance, VkSurfaceKHR surface)
	: m_CommandQueue(new std::remove_pointer<decltype(m_CommandQueue)>::type)
	, m_Instance(instance)
{
	InitDevice(surface);
}

//=================================================================================
C_VkRenderer::~C_VkRenderer()
{
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
void C_VkRenderer::AddBatch(T_BatchPtr)
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
void C_VkRenderer::SortCommands()
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
void C_VkRenderer::ExtractData()
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
void C_VkRenderer::TransformData()
{
	throw std::logic_error("The method or operation is not implemented.");
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
	m_GPU = gpus[0];

	{
		uint32_t familyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(m_GPU, &familyCount, nullptr);
		std::vector<VkQueueFamilyProperties> familyProperties(familyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(m_GPU, &familyCount, familyProperties.data());

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

			VkBool32   presentSupport = false;
			const auto ret			  = vkGetPhysicalDeviceSurfaceSupportKHR(m_GPU, i, surface, &presentSupport);
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

	VkDeviceCreateInfo device_create_info{};
	device_create_info.sType				   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_create_info.pNext				   = nullptr;
	device_create_info.queueCreateInfoCount	   = static_cast<uint32_t>(queueInfos.size());
	device_create_info.pQueueCreateInfos	   = queueInfos.data();
	device_create_info.pEnabledFeatures		   = &device_features;
	device_create_info.enabledExtensionCount   = static_cast<uint32_t>(deviceExtensions.size());
	device_create_info.ppEnabledExtensionNames = deviceExtensions.data();


	auto ret = pfnCreateDevice(m_GPU, &device_create_info, nullptr, &m_VkDevice);
	if (ret != VK_SUCCESS)
	{
		CORE_LOG(E_Level::Error, E_Context::Core, "Vulkan device unable to initialize. Error code: '{}'", ret);
		std::exit(EXIT_FAILURE);
	}

	vkGetDeviceQueue(m_VkDevice, m_GraphicsFamilyIndex, 0, &m_graphicsQueue);
	vkGetDeviceQueue(m_VkDevice, m_PresentingFamilyIndex, 0, &m_presentQueue);

	m_Device.Init(m_VkDevice);

	return true;
}

//=================================================================================
std::vector<VkDeviceQueueCreateInfo> C_VkRenderer::CreatePresentingQueueInfos()
{
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t>					 uniqueQueueFamilies = {m_GraphicsFamilyIndex, m_PresentingFamilyIndex};

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
SwapChainSupportDetails C_VkRenderer::QuerySwapChainSupport(VkSurfaceKHR sruface)
{
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_GPU, sruface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_GPU, sruface, &formatCount, nullptr);

	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_GPU, sruface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(m_GPU, sruface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_GPU, sruface, &presentModeCount, details.presentModes.data());
	}
	return details;
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

} // namespace GLEngine::VkRenderer
