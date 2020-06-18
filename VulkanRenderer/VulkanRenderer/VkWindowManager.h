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
protected:
	virtual void Init() override;

	bool InitDevice();
	void SetupDebug();
	void InitDebug();

	void CheckLayersSupport();

	VkInstance_T*		m_Instance;
	VkDevice_T*			m_Device;
	VkPhysicalDevice_T* m_GPU;
	VkDebugUtilsMessengerEXT_T* m_DebugMessenger;

	uint32_t m_GraphicsFamilyIndex;
	uint32_t m_ComputeFamilyIndex;

	std::vector<const char*> m_instance_layer_list;
	std::vector<const char*> m_instance_extensions_list;
};
}