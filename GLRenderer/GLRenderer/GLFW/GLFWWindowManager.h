#pragma once

#include <GLRenderer/GLRendererApi.h>

#include <Core/IWindowManager.h>
#include <GLRenderer/GLFW/GLFWWindow.h>

#include <vector>

namespace GLEngine {
namespace GLRenderer {
namespace GLFW {

class C_GLFWWindowManager : public Core::I_WindowManager {
public:
	C_GLFWWindowManager(Core::C_Application::EventCallbackFn eventCallback);
	//=============================================================
	// I_WindowManager
	//=============================================================
	virtual std::shared_ptr<Core::I_Window> OpenNewWindow(const Core::S_WindowInfo& info) override;
	virtual std::shared_ptr<Core::I_Window> GetWindow(GUID guid) const override;
	virtual void Update() override;
	virtual unsigned int NumWindows() const override;


	//=================================================================================
	// Core::C_Layer
	//=================================================================================
	virtual void OnEvent(Core::I_Event& event) override;

protected:
	void Init();

private:
	std::vector<std::shared_ptr<Core::I_Window>> m_Windows;
};

GL_RENDERER_API_EXPORT Core::I_WindowManager* ConstructGLFWManager(Core::C_Application::EventCallbackFn eventCallback);
}}}