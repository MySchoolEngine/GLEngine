#pragma once

#include <Core/Input.h>
#include <Core/InputDefinitions.h>

#include <GLFWWindowManager/GLFWWindowManagerApi.h>

struct GLFWwindow;
struct GLFWcursor;

namespace GLEngine::GLFWManager {

class GL_GLFWWINDOWMANAGER_API_EXPORT C_GLFWInput : public Core::I_Input {
public:
	C_GLFWInput();
	virtual ~C_GLFWInput();

	void SetWindow(GLFWwindow* window) { m_Window = window; }
	//==========================================================
	// Core::I_Input
	//==========================================================
	[[nodiscard]] virtual bool IsKeyPressed(int keycode) const override;
	[[nodiscard]] virtual bool IsMouseButtonPressed(int key) const override;
	/**
	 * Returns mouse position in screen space
	 * Top left = (0,0)
	 */
	[[nodiscard]] virtual std::pair<float, float> GetMousePosition() const override;
	[[nodiscard]] virtual glm::vec2				  GetClipSpaceMouseCoord() const override;
	[[nodiscard]] virtual float					  GetMouseX() const override;
	[[nodiscard]] virtual float					  GetMouseY() const override;


	virtual void SetMouseCursor(E_MouseCursor cursor) override;

protected:
	GLFWwindow*																m_Window;
	std::array<GLFWcursor*, static_cast<std::size_t>(E_MouseCursor::Count)> m_MouseCursors;
};

[[nodiscard]] ::Utils::C_BitField<Core::E_KeyModifiers> TranslateGLFWModifiers(int mods);

} // namespace GLEngine::GLFWManager