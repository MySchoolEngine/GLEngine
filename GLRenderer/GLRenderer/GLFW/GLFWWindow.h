#pragma once

#include <GLRenderer/GLFW/GLFWInput.h>

#include <Core/Application.h>
#include <Core/IWindow.h>

struct GLFWwindow;

namespace GLEngine {
namespace Core {
class I_Input;
}

namespace GLRenderer::GLFW {
class C_GLFWWindow : public Core::I_Window {
public:
	virtual ~C_GLFWWindow();

	//============================================================
	// I_Window
	//============================================================
	virtual unsigned int GetWidth() const override;
	virtual unsigned int GetHeight() const override;
	virtual glm::uvec2	 GetSize() const override;
	virtual void		 SetTitle(const std::string& title) override;
	virtual void		 Update() override;
	virtual bool		 WantClose() const override;

	virtual void Init(const Core::S_WindowInfo& wndInfo) override;


	//=================================================================================
	virtual const Core::I_Input& GetInput() const override;


	virtual glm::vec2 ToClipSpace(const glm::vec2& screenCoord) const override;

protected:
	C_GLFWWindow();
	virtual void Destroy() override;

	GLFWwindow* m_Window;
	C_GLFWInput m_Input;
};
} // namespace GLRenderer::GLFW
} // namespace GLEngine