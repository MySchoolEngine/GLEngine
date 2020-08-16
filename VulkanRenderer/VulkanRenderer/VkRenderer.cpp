#include <VkRendererStdAfx.h>

#include <VulkanRenderer/VkRenderer.h>

#include <Renderer/IRenderBatch.h>
#include <Renderer/IRenderCommand.h>

namespace GLEngine::VkRenderer {

//=================================================================================
C_VkRenderer::C_VkRenderer(VkInstance instance, GLFWwindow* window)
	: m_CommandQueue(new std::remove_pointer<decltype(m_CommandQueue)>::type)
	, m_Instance(instance)
	, m_Window(window)
{
	CreateWindowSurface();
	InitDevice();
	CreateSwapChain();
	CreateImageViews();
}

//=================================================================================
C_VkRenderer::~C_VkRenderer()
{
	for (auto imageView : m_SwapChainImagesViews) {
		vkDestroyImageView(m_Device, imageView, nullptr);
	}
	vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);
	vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
	vkDestroyDevice(m_Device, nullptr);
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
	if (m_Locked) {
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
	for (auto& command : (*m_CommandQueue)) {
		command->Commit();
	}
}

//=================================================================================
void C_VkRenderer::ClearCommandBuffers()
{
	m_CommandQueue->clear();
}

//=================================================================================
bool C_VkRenderer::InitDevice()
{
	auto pfnCreateDevice = (PFN_vkCreateDevice)
		glfwGetInstanceProcAddress(m_Instance, "vkCreateDevice");

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

		uint32_t i = 0;
		bool found = false;
		for (auto it : familyProperties)
		{
			if (it.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				m_GraphicsFamilyIndex = i;
				found = true;
			}
			else if (it.queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
				m_ComputeFamilyIndex = i;
			}

			VkBool32 presentSupport = false;
			const auto ret = vkGetPhysicalDeviceSurfaceSupportKHR(m_GPU, i, m_Surface, &presentSupport);
			if (presentSupport) {
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

	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	const auto queueInfos = CreatePresentingQueueInfos();

	VkPhysicalDeviceFeatures device_features{};

	VkDeviceCreateInfo device_create_info{};
	device_create_info.sType					= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_create_info.pNext					= nullptr;
	device_create_info.queueCreateInfoCount		= static_cast<uint32_t>(queueInfos.size());
	device_create_info.pQueueCreateInfos		= queueInfos.data();
	device_create_info.pEnabledFeatures			= &device_features;
	device_create_info.enabledExtensionCount	= static_cast<uint32_t>(deviceExtensions.size());
	device_create_info.ppEnabledExtensionNames	= deviceExtensions.data();


	auto ret = pfnCreateDevice(m_GPU, &device_create_info, nullptr, &m_Device);
	if (ret != VK_SUCCESS)
	{
		CORE_LOG(E_Level::Error, E_Context::Core, "Vulkan device unable to initialize. Error code: '{}'", ret);
		std::exit(EXIT_FAILURE);
	}

	vkGetDeviceQueue(m_Device, m_GraphicsFamilyIndex, 0, &m_graphicsQueue);
	vkGetDeviceQueue(m_Device, m_PresentingFamilyIndex, 0, &m_presentQueue);

	return true;
}

//=================================================================================
bool C_VkRenderer::CreateWindowSurface()
{
	const auto ret = glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface);
	if (ret != VK_SUCCESS)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Window unable to initialise its surface");
		return false;
	}
	return true;
}

//=================================================================================
std::vector<VkDeviceQueueCreateInfo> C_VkRenderer::CreatePresentingQueueInfos()
{
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { m_GraphicsFamilyIndex, m_PresentingFamilyIndex };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType				= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex	= queueFamily;
		queueCreateInfo.queueCount			= 1;
		queueCreateInfo.pQueuePriorities	= &queuePriority;


		queueCreateInfos.push_back(queueCreateInfo);
	}

	return queueCreateInfos;
}

//=================================================================================
C_VkRenderer::SwapChainSupportDetails C_VkRenderer::QuerySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, details.presentModes.data());
	}
	return details;
}

//=================================================================================
VkSurfaceFormatKHR C_VkRenderer::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

//=================================================================================
VkPresentModeKHR C_VkRenderer::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

//=================================================================================
VkExtent2D C_VkRenderer::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, VkExtent2D actualExtent) {
	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	}
	else {
		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

//=================================================================================
void C_VkRenderer::CreateSwapChain() {
	const SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(m_GPU);

	const VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	const VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
	glm::ivec2 size;
	glfwGetWindowSize(m_Window, &(size.x), &(size.y));
	const VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities, { static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y) });

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_Surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	uint32_t queueFamilyIndices[] = { m_GraphicsFamilyIndex, m_PresentingFamilyIndex };

	if (m_GraphicsFamilyIndex != m_PresentingFamilyIndex) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

	// blending with other windows
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	// clipping the pixels obstructed by other windows
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(m_Device, &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS) {
		CORE_LOG(E_Level::Error, E_Context::Render, "failed to create swap chain");
		return;
	}


	vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, nullptr);
	m_SwapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, m_SwapChainImages.data());

	m_SwapChainImageFormat = surfaceFormat.format;
	m_SwapChainExtent = extent;
}

//=================================================================================
void C_VkRenderer::CreateImageViews()
{
	m_SwapChainImagesViews.resize(m_SwapChainImages.size());

	std::size_t i = 0;
	for (const auto& image : m_SwapChainImages)
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = image;

		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = m_SwapChainImageFormat;

		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(m_Device, &createInfo, nullptr, &m_SwapChainImagesViews[i]) != VK_SUCCESS) {
			CORE_LOG(E_Level::Error, E_Context::Render, "failed to create image views");
			return;
		}

		++i;
	}
}

//=================================================================================
Renderer::E_PassType C_VkRenderer::GetCurrentPassType() const
{
	return Renderer::E_PassType::FinalPass;
}

}
