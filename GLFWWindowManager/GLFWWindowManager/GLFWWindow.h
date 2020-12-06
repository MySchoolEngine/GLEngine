#pragma once

#include <GLFWWindowManager/GLFWWindowManagerApi.h>

#include <Core/IWindow.h>

#include <GLFWWindowManager/GLFWInput.h>

struct GLFWwindow;

namespace GLEngine::GLFWManager {
class C_GLFWWindow : public Core::I_Window {
public:
	virtual ~C_GLFWWindow();

	//============================================================
	// I_Window
	//============================================================
	[[nodiscard]] virtual unsigned int GetWidth() const override;
	[[nodiscard]] virtual unsigned int GetHeight() const override;
	[[nodiscard]] virtual glm::uvec2 GetSize() const override;
	virtual void SetTitle(const std::string& title) override;
	virtual void Update() override;
	[[nodiscard]] virtual bool WantClose() const override;

	virtual void Init(const Core::S_WindowInfo& wndInfo) override;


	//=================================================================================
	[[nodiscard]] virtual const Core::I_Input& GetInput() const override;


	virtual glm::vec2 ToClipSpace(const glm::vec2& screenCoord) const override;

protected:
	C_GLFWWindow();
	virtual void Destroy() override;

	void MakeCurrent();
	void WindowHint(int hint, int value);

	GLFWwindow* m_Window;
	C_GLFWInput m_Input;
};
}