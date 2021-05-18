#include <VulkanRendererStdafx.h>

#include <VulkanRenderer/VkRenderer.h>
#include <VulkanRenderer/VkWindow.h>
#include <VulkanRenderer/VkWindowInfo.h>

#include <Core/EventSystem/Event/AppEvent.h>
#include <Core/EventSystem/EventDispatcher.h>

#include <GLFW/glfw3.h>

namespace GLEngine::VkRenderer {

//=================================================================================
C_VkWindow::C_VkWindow(const Core::S_WindowInfo& wndInfo)
	: m_renderer(nullptr)
	, m_Instance(nullptr)
{
	Init(wndInfo);

	CreateWindowSurface();
	m_renderer = std::make_unique<C_VkRenderer>(m_Instance, m_Surface);

	CORE_LOG(E_Level::Info, E_Context::Render, "GLFW: Vulkan window initialized");
	CreateSwapChain();
	CreateImageViews();
}

//=================================================================================
C_VkWindow::~C_VkWindow()
{
	DestroySwapchain();
	vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
	m_renderer.reset(nullptr);
};

//=================================================================================
void C_VkWindow::DestroySwapchain()
{
	for (auto imageView : m_SwapChainImagesViews)
	{
		vkDestroyImageView(m_renderer->GetDevice(), imageView, nullptr);
	}
	vkDestroySwapchainKHR(m_renderer->GetDevice(), m_SwapChain, nullptr);
}

//=================================================================================
void C_VkWindow::Init(const Core::S_WindowInfo& wndInfo)
{
	GLE_ASSERT(wndInfo.GetDriver() == Core::E_Driver::Vulkan, "This class supports only OpenGL");

	const auto vkInfo = dynamic_cast<const S_VkWindowInfo*>(&wndInfo);
	SetLayerDebugName(wndInfo.m_name);
	m_Instance = vkInfo->m_Instance;

	WindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	C_GLFWWindow::Init(wndInfo);
}

//=================================================================================
Renderer::I_Renderer& C_VkWindow::GetRenderer()
{
	return *(m_renderer.get());
}

//=================================================================================
bool C_VkWindow::CreateWindowSurface()
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
void C_VkWindow::CreateSwapChain()
{
	const SwapChainSupportDetails swapChainSupport = m_renderer->QuerySwapChainSupport(m_Surface);

	const VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	const VkPresentModeKHR	 presentMode   = ChooseSwapPresentMode(swapChainSupport.presentModes);
	glm::ivec2				 size		   = GetSize();
	const VkExtent2D		 extent		   = ChooseSwapExtent(swapChainSupport.capabilities, {static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y)});

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType   = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_Surface;

	createInfo.minImageCount	= imageCount;
	createInfo.imageFormat		= surfaceFormat.format;
	createInfo.imageColorSpace	= surfaceFormat.colorSpace;
	createInfo.imageExtent		= extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage		= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	m_renderer->FillFamilyIndexes(createInfo);

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

	// blending with other windows
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	// clipping the pixels obstructed by other windows
	createInfo.presentMode = presentMode;
	createInfo.clipped	   = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(m_renderer->GetDevice(), &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "failed to create swap chain");
		return;
	}


	vkGetSwapchainImagesKHR(m_renderer->GetDevice(), m_SwapChain, &imageCount, nullptr);
	m_SwapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(m_renderer->GetDevice(), m_SwapChain, &imageCount, m_SwapChainImages.data());

	m_SwapChainImageFormat = surfaceFormat.format;
	m_SwapChainExtent	   = extent;
}

//=================================================================================
VkSurfaceFormatKHR C_VkWindow::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	const auto it = std::find_if(availableFormats.begin(), availableFormats.end(), [](const auto& availableFormat) {
		return availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	});
	if (it != availableFormats.end())
		return *it;

	return availableFormats[0];
}

//=================================================================================
VkPresentModeKHR C_VkWindow::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	const auto it = std::find_if(availablePresentModes.begin(), availablePresentModes.end(),
								 [](const auto& availablePresentMode) { return availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR; });
	if (it != availablePresentModes.end())
		return *it;

	return VK_PRESENT_MODE_FIFO_KHR;
}

//=================================================================================
VkExtent2D C_VkWindow::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, VkExtent2D actualExtent)
{
	if (capabilities.currentExtent.width != UINT32_MAX)
	{
		return capabilities.currentExtent;
	}
	else
	{
		actualExtent.width	= std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

//=================================================================================
void C_VkWindow::CreateImageViews()
{
	m_SwapChainImagesViews.resize(m_SwapChainImages.size());

	std::size_t i = 0;
	for (const auto& image : m_SwapChainImages)
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = image;

		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format	= m_SwapChainImageFormat;

		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		createInfo.subresourceRange.aspectMask	   = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel   = 0;
		createInfo.subresourceRange.levelCount	   = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount	   = 1;

		if (vkCreateImageView(m_renderer->GetDevice(), &createInfo, nullptr, &m_SwapChainImagesViews[i]) != VK_SUCCESS)
		{
			CORE_LOG(E_Level::Error, E_Context::Render, "failed to create image views");
			return;
		}

		++i;
	}
}

//=================================================================================
void C_VkWindow::OnEvent(Core::I_Event& event)
{
	// base can make filtering
	GLFWManager::C_GLFWWindow::OnEvent(event);

	Core::C_EventDispatcher d(event);
	d.Dispatch<Core::C_WindowResizedEvent>(std::bind(&C_VkWindow::OnWindowResized, this, std::placeholders::_1));
}

//=================================================================================
bool C_VkWindow::OnWindowResized(Core::C_WindowResizedEvent& event)
{
	DestroySwapchain();
	CreateSwapChain();
	return true;
}

} // namespace GLEngine::VkRenderer
