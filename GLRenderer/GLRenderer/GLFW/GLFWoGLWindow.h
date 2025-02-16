#pragma once

#include <Renderer/IRenderer.h>

#include <GLFWWindowManager/GLFWWindow.h>

namespace GLEngine {

namespace Core {
class C_KeyPressedEvent;
}

namespace GLRenderer {
class C_GLDevice;
}

namespace GLRenderer::GLFW {

class C_GLFWoGLWindow : public GLFWManager::C_GLFWWindow {
public:
	explicit C_GLFWoGLWindow(const Core::S_WindowInfo& wndInfo);
	~C_GLFWoGLWindow() override;

	[[nodiscard]] Renderer::I_Renderer& GetRenderer() override;
	void								OnEvent(Core::I_Event& event) override;

protected:
	void								  Init(const Core::S_WindowInfo& wndInfo) override;
	std::unique_ptr<C_GLDevice>			  m_Device;
	std::unique_ptr<Renderer::I_Renderer> m_renderer;
};

} // namespace GLRenderer::GLFW
} // namespace GLEngine
