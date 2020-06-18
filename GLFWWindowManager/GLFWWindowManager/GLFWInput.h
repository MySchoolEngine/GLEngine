#pragma once

#include <Core/Input.h>

struct GLFWwindow;

namespace GLEngine::GLFWManager {

class GL_GLFWWINDOWMANAGER_API_EXPORT C_GLFWInput : public Core::I_Input {
public:
	C_GLFWInput();
	virtual ~C_GLFWInput() = default;

	void SetWindow(GLFWwindow* window) { m_Window = window; }
	//==========================================================
	// Core::I_Input
	//==========================================================
	[[nodiscard]] virtual bool IsKeyPressed(int keycode) const override;
	[[nodiscard]] virtual bool IsMosueButtonPresse(int key) const override;
	[[nodiscard]] virtual std::pair<float, float> GetMousePosition() const override;
	[[nodiscard]] virtual float GetMouseX() const override;
	[[nodiscard]] virtual float GetMouseY() const override;

protected:
	GLFWwindow* m_Window;
};

}
