#include <VkRendererStdafx.h>

#include <VulkanRenderer/VkWindow.h>
#include <VulkanRenderer/VkWindowFactory.h>
#include <VulkanRenderer/VkWindowManager.h>

namespace GLEngine::VkRenderer {

//=================================================================================
std::shared_ptr<Core::I_Window> C_VkWindowFactory::GetWindow(const Core::S_WindowInfo& wndInfo) const
{
	if (wndInfo.m_WindowClass == "VkWindow")
	{
		return std::make_shared<C_VkWindow>(wndInfo);
	}
	return nullptr;
}

//=================================================================================
C_VkWindowFactory::C_VkWindowFactory() = default;

//=================================================================================
Core::I_WindowFactory* ConstructVkWindowFactory()
{
	return new C_VkWindowFactory();
}

//=================================================================================
VK_RENDERER_API_EXPORT C_VkWindowManager* ConstructVkWManager(Core::C_Application::EventCallbackFn eventCallback)
{
	return new C_VkWindowManager(eventCallback);
}

} // namespace GLEngine::VkRenderer