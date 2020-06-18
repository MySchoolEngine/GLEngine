#include <VkRendererStdafx.h>

#include <VulkanRenderer/VkWindow.h>

#include <VulkanRenderer/VkWindowInfo.h>
#include <VulkanRenderer/VkRenderer.h>

namespace GLEngine::VkRenderer {

//=================================================================================
C_VkWindow::C_VkWindow(const Core::S_WindowInfo& wndInfo)
	: m_renderer(nullptr)
{
	Init(wndInfo);
}

//=================================================================================
C_VkWindow::~C_VkWindow() = default;

//=================================================================================
void C_VkWindow::Init(const Core::S_WindowInfo& wndInfo)
{
	GLE_ASSERT(wndInfo.GetDriver() == Core::E_Driver::Vulkan, "This class supports only OpenGL");

	const auto wndInfoOGL = dynamic_cast<const S_VkWindowInfo*>(&wndInfo);
	SetLayerDebugName(wndInfo.m_name);
	
	WindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	C_GLFWWindow::Init(wndInfo);
	MakeCurrent();

	m_renderer = std::make_unique<VkRenderer::C_VkRenderer>();

	CORE_LOG(E_Level::Info, E_Context::Render, "GLFW: Vulkan window initialized");
}

//=================================================================================
const std::unique_ptr<Renderer::I_Renderer>& C_VkWindow::GetRenderer() const
{
	return m_renderer;
}

}
