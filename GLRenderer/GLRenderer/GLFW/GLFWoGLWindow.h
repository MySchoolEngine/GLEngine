#pragma once

#include <GLFWWindowManager/GLFWWindow.h>

#include <Renderer/IRenderer.h>

#include <glm/glm.hpp>

#include <memory>

namespace GLEngine {

namespace Core {
class C_KeyPressedEvent;
}

namespace GLRenderer::GLFW {

class C_GLFWoGLWindow : public GLFWManager::C_GLFWWindow {
public:
	C_GLFWoGLWindow(const Core::S_WindowInfo& wndInfo);
	virtual ~C_GLFWoGLWindow();

	virtual const std::unique_ptr<Renderer::I_Renderer>& GetRenderer() const override;
protected:
	virtual void Init(const Core::S_WindowInfo& wndInfo) override;
	std::unique_ptr<Renderer::I_Renderer> m_renderer;
};

}
}

