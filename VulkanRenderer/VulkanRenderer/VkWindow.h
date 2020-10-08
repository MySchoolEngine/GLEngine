#pragma once

#include <Core/IWindow.h>
#include <Core/Application.h>

#include <GLFWWindowManager/GLFWWindow.h>

struct GLFWwindow;

namespace GLEngine::VkRenderer {

class C_VkRenderer;

class C_VkWindow : public GLFWManager::C_GLFWWindow {
public:
	C_VkWindow(const Core::S_WindowInfo& wndInfo);
	virtual ~C_VkWindow();

	virtual Renderer::I_Renderer& GetRenderer() override;

protected:
	virtual void Init(const Core::S_WindowInfo& wndInfo) override;
	std::unique_ptr<C_VkRenderer> m_renderer;

	VkInstance_T* m_Instance;
};
}
