#pragma once

#include <GLFWWindowManager/GLFWWindowManager.h>

struct VkInstance_T;
struct VkDevice_T;
struct VkPhysicalDevice_T;
struct VkDebugUtilsMessengerEXT_T;

namespace GLEngine::VkRenderer {

class C_VkWindowManager : public GLFWManager::C_GLFWWindowManager {
public:
	C_VkWindowManager(Core::C_Application::EventCallbackFn eventCallback);
	~C_VkWindowManager();

	//=================================================================================
	std::shared_ptr<Core::I_Window> OpenNewWindow(const Core::S_WindowInfo& info) override;

protected:
	virtual void Init() override;

	void SetupDebug();
	void InitDebug();

	void CheckLayersSupport();

	VkInstance_T*				m_Instance;
	VkDebugUtilsMessengerEXT_T* m_DebugMessenger;

	std::vector<const char*> m_instance_layer_list;
	std::vector<const char*> m_instance_extensions_list;
};
} // namespace GLEngine::VkRenderer