#include <VulkanRendererStdafx.h>

#include <VulkanRenderer/VkWindow.h>
#include <VulkanRenderer/VkWindowFactory.h>
#include <VulkanRenderer/VkWindowInfo.h>
#include <VulkanRenderer/VkWindowManager.h>

#include <Renderer/IRenderer.h>

namespace GLEngine::VkRenderer {

//=================================================================================
C_VkWindowManager::C_VkWindowManager(Core::C_Application::EventCallbackFn eventCallback)
	: GLFWManager::C_GLFWWindowManager(eventCallback, Core::E_Driver::Vulkan)
	, m_Instance(nullptr)
{
	Init();
}

//=================================================================================
void C_VkWindowManager::Init()
{
	GLFWManager::C_GLFWWindowManager::Init();

	if (!glfwVulkanSupported())
	{
		CORE_LOG(E_Level::Error, E_Context::Core, "GLFW: Vulkan is not available. Terminating engine");
		exit(EXIT_FAILURE);
	}

#ifdef GL_ENGINE_DEBUG
	SetupDebug();
#endif
	CheckLayersSupport();

	const VkApplicationInfo application_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO,
												nullptr,
												"GLEngine", // a bit ironical isn't it?
												VK_MAKE_VERSION(0, 1, 0),
												nullptr,
												VK_MAKE_VERSION(0, 1, 0),
												VK_MAKE_VERSION(1, 0, 3)};

	const VkInstanceCreateInfo instance_create_info = {
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		nullptr,		   // No pointer to the next thing (?)
		0,				   // No creation flags
		&application_info, // No application info
		static_cast<uint32_t>(m_instance_layer_list.size()),
		m_instance_layer_list.data(),
		static_cast<uint32_t>(m_instance_extensions_list.size()), // The number of extensions
		m_instance_extensions_list.data()						  // The extensions to load
	};

	const auto res = vkCreateInstance(&instance_create_info, nullptr, &m_Instance);
	if (res != VK_SUCCESS)
	{
		CORE_LOG(E_Level::Error, E_Context::Core, "Vulkan instance unable to initialize. Error code: '{}'", res);
		std::exit(EXIT_FAILURE);
	}

#ifdef GL_ENGINE_DEBUG
	InitDebug();
#endif
}

//=================================================================================
C_VkWindowManager::~C_VkWindowManager()
{
	const auto pfnvkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT");
	GLE_ASSERT(pfnvkDestroyDebugUtilsMessengerEXT, "GLFW: havent found vkDestroyDebugUtilsMessengerEXT");
	pfnvkDestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);

	vkDestroyInstance(m_Instance, nullptr);
	m_Instance = nullptr;
}

//=================================================================================
void C_VkWindowManager::SetupDebug()
{
	// todo cherry pick layers one day
	m_instance_layer_list.emplace_back("VK_LAYER_KHRONOS_validation");

	m_instance_extensions_list.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	m_instance_extensions_list.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
	m_instance_extensions_list.emplace_back("VK_KHR_win32_surface");
}

//=================================================================================
VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugReportCallback(VkDebugUtilsMessageSeverityFlagBitsEXT		 messageSeverity,
														 VkDebugUtilsMessageTypeFlagsEXT			 messageType,
														 const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
														 void*										 pUserData)
{
	CORE_LOG(E_Level::Error, E_Context::Render, "Vulkan: {}", pCallbackData->pMessage);

	return VK_FALSE;
}

//=================================================================================
void C_VkWindowManager::InitDebug()
{
	const auto pfnCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT");
	GLE_ASSERT(pfnCreateDebugUtilsMessengerEXT, "Vulkan: havent found vkCreateDebugUtilsMessengerEXT");

	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	createInfo.sType		   = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType	   = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = VulkanDebugReportCallback;
	createInfo.pUserData	   = nullptr;

	pfnCreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger);
}

//=================================================================================
void C_VkWindowManager::CheckLayersSupport(){
#ifdef GL_ENGINE_DEBUG
	{uint32_t layerCount = 0;
vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
std::vector<VkLayerProperties> layers(layerCount);
vkEnumerateInstanceLayerProperties(&layerCount, layers.data());
CORE_LOG(E_Level::Info, E_Context::Render, "Instance layers: ");
for (const auto it : layers)
{
	CORE_LOG(E_Level::Info, E_Context::Render, "\t{}: {}", it.layerName, it.description);
}
}
#endif
}

//=================================================================================
std::shared_ptr<GLEngine::Core::I_Window> C_VkWindowManager::OpenNewWindow(const Core::S_WindowInfo& info)
{
	if (info.GetDriver() != Core::E_Driver::Vulkan)
	{
		return nullptr;
	}
	auto* vkwndInfo		  = (const_cast<S_VkWindowInfo*>(dynamic_cast<const S_VkWindowInfo*>(&info)));
	vkwndInfo->m_Instance = m_Instance;

	return GLFWManager::C_GLFWWindowManager::OpenNewWindow(info);
}

//=================================================================================
// ConstructVkManager
//=================================================================================
C_VkWindowManager* ConstructVkManager(Core::C_Application::EventCallbackFn eventCallback)
{
	return new C_VkWindowManager(eventCallback);
}
}