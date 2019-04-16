#pragma once

#include <Core/Input.h>

struct GLFWwindow;

namespace GLEngine {
namespace GLRenderer {
namespace GLFW {

class API_EXPORT C_GLFWInput : public Core::I_Input {
public:
	C_GLFWInput();
	virtual ~C_GLFWInput() = default;

	void SetWindow(GLFWwindow* window) { m_Window = window; }
	//==========================================================
	// Core::I_Input
	//==========================================================
	virtual bool IsKeyPressed(int keycode) const override;
	virtual bool IsMosueButtonPresse(int key) const override;
	virtual std::pair<float, float> GetMousePosition() const override;
	virtual float GetMouseX() const override;
	virtual float GetMouseY() const override;

protected:
	GLFWwindow* m_Window;
};

}}}