#pragma once

#include <GLFWWindowManager/GLFWWindow.h>

#include <Renderer/IRenderer.h>

namespace GLEngine {

namespace Core {
class C_KeyPressedEvent;
}

namespace GLRenderer::GLFW {

class C_GLFWoGLWindow : public GLFWManager::C_GLFWWindow {
public:
	explicit C_GLFWoGLWindow(const Core::S_WindowInfo& wndInfo);
	virtual ~C_GLFWoGLWindow();
	virtual void Update() override;

	[[nodiscard]] virtual Renderer::I_Renderer& GetRenderer() override;
	virtual void OnEvent(Core::I_Event& event) override;
protected:
	virtual void Init(const Core::S_WindowInfo& wndInfo) override;
	std::unique_ptr<Renderer::I_Renderer> m_renderer;
};

}
}

