#include <VkRendererStdafx.h>

#include <VulkanRenderer/VkWindowManager.h>

#include <VulkanRenderer/VkWindow.h>
#include <VulkanRenderer/VkWindowFactory.h>
#include <VulkanRenderer/VkWindowInfo.h>

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

	const VkApplicationInfo application_info = {
		VK_STRUCTURE_TYPE_APPLICATION_INFO,
		nullptr,
		"GLEngine",// a bit ironical isn't it?
		VK_MAKE_VERSION(0, 1, 0),
		nullptr,
		VK_MAKE_VERSION(0, 1, 0),
		VK_MAKE_VERSION(1, 0, 3)
	};

	const VkInstanceCreateInfo instance_create_info = {
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		nullptr,						// No pointer to the next thing (?)
		0,								// No creation flags
		&application_info,				// No application info
		static_cast<uint32_t>(m_instance_layer_list.size()),
		m_instance_layer_list.data(),
		static_cast<uint32_t>(m_instance_extensions_list.size()),	// The number of extensions
		m_instance_extensions_list.data()	// The extensions to load
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
	InitDevice();
}

//=================================================================================
C_VkWindowManager::~C_VkWindowManager()
{
	const auto pfnvkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT");
	GLE_ASSERT(pfnvkDestroyDebugUtilsMessengerEXT, "GLFW: havent found vkDestroyDebugUtilsMessengerEXT");
	pfnvkDestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);

	vkDestroyDevice(m_Device, nullptr);
	vkDestroyInstance(m_Instance, nullptr);
	m_Instance = nullptr;

}

//=================================================================================
bool C_VkWindowManager::InitDevice()
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
			++i;
		}
		if (!found)
		{
			GLE_ASSERT(found, "Vulkan: Not found queue supporting graphics.");
			std::exit(EXIT_FAILURE);
		}
	}

	float queue_priorities[]{ 1.0f };
	VkDeviceQueueCreateInfo device_queue_create_info{};
	device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	device_queue_create_info.pNext = nullptr;
	device_queue_create_info.flags = 0;
	device_queue_create_info.queueFamilyIndex = m_GraphicsFamilyIndex;
	device_queue_create_info.queueCount = 1;
	device_queue_create_info.pQueuePriorities = queue_priorities;

	VkDeviceCreateInfo device_create_info{};
	device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_create_info.pNext = nullptr;
	device_create_info.queueCreateInfoCount = 1;
	device_create_info.pQueueCreateInfos = &device_queue_create_info;
	

	auto ret = pfnCreateDevice(m_GPU, &device_create_info, nullptr, &m_Device);
	if (ret != VK_SUCCESS)
	{
		CORE_LOG(E_Level::Error, E_Context::Core, "Vulkan device unable to initialize. Error code: '{}'", ret);
		std::exit(EXIT_FAILURE);
	}

	return false;
}

//=================================================================================
void C_VkWindowManager::SetupDebug()
{
	// todo cherry pick layers one day
	m_instance_layer_list.emplace_back("VK_LAYER_KHRONOS_validation");

	m_instance_extensions_list.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
}

//=================================================================================
VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugReportCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData
)
{
	CORE_LOG(E_Level::Error, E_Context::Render, "Vulkan: {}", pCallbackData->pMessage);

	return VK_FALSE;
}

//=================================================================================
void C_VkWindowManager::InitDebug()
{
	const auto pfnCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)
		vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT");
	GLE_ASSERT(pfnCreateDebugUtilsMessengerEXT, "Vulkan: havent found vkCreateDebugUtilsMessengerEXT");

	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = VulkanDebugReportCallback;
	createInfo.pUserData = nullptr;

	pfnCreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger);
}

//=================================================================================
void C_VkWindowManager::CheckLayersSupport()
{
#ifdef GL_ENGINE_DEBUG
	{
		uint32_t layerCount = 0;
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
// ConstructVkManager
//=================================================================================
C_VkWindowManager* ConstructVkManager(Core::C_Application::EventCallbackFn eventCallback)
{
	return new C_VkWindowManager(eventCallback);
}

}