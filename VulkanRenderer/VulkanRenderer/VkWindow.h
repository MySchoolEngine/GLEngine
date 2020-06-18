#pragma once

#include <Core/IWindow.h>
#include <Core/Application.h>

#include <GLFWWindowManager/GLFWWindow.h>

namespace GLEngine::VkRenderer {

class C_VkWindow : public GLFWManager::C_GLFWWindow {
public:
	C_VkWindow(const Core::S_WindowInfo& wndInfo);
	virtual ~C_VkWindow();

	virtual const std::unique_ptr<Renderer::I_Renderer>& GetRenderer() const override;

protected:
	virtual void Init(const Core::S_WindowInfo& wndInfo) override;
	std::unique_ptr<Renderer::I_Renderer> m_renderer;
};
}
