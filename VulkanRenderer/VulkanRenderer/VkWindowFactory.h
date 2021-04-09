#pragma once

#include <VulkanRenderer/VkRendererApi.h>

#include <Core/IWindowManager.h>


namespace GLEngine::VkRenderer {
class C_VkWindowManager;

class C_VkWindowFactory final : public Core::I_WindowFactory {
public:
	C_VkWindowFactory();
	virtual std::shared_ptr<Core::I_Window> GetWindow(const Core::S_WindowInfo&) const override;
};

VK_RENDERER_API_EXPORT Core::I_WindowFactory* ConstructVkWindowFactory();
VK_RENDERER_API_EXPORT C_VkWindowManager* ConstructVkWManager(Core::C_Application::EventCallbackFn eventCallback);
} // namespace GLEngine::VkRenderer