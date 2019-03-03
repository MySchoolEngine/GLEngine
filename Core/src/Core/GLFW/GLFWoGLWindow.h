#pragma once

#include <Core/GLFW/GLFWWindow.h>

#include <glm/glm.hpp>

namespace Core {
namespace GLFW {

class API_EXPORT C_GLFWoGLWindow : public C_GLFWWindow {
public:
	C_GLFWoGLWindow();
	virtual ~C_GLFWoGLWindow() = default;
	virtual void Update() override;

protected:
	virtual void Init() override;
private:
	float m_color = 0.0f;
};


}
}

