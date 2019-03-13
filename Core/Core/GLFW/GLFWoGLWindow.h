#pragma once

#include <Core/GLFW/GLFWWindow.h>

#include <Renderer/IRenderer.h>

#include <glm/glm.hpp>

#include <memory>

namespace Core {
namespace GLFW {

class C_GLFWoGLWindow : public C_GLFWWindow {
public:
	C_GLFWoGLWindow(const S_WindowInfo& wndInfo);
	virtual ~C_GLFWoGLWindow() = default;
	virtual void Update() override;

protected:
	virtual void Init(const S_WindowInfo& wndInfo) override;
private:
	float m_color = 0.0f;
	std::unique_ptr<GLEngine::Renderer::I_Renderer> m_renderer;
};


}
}

