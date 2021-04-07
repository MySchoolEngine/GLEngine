#pragma once

#include <GLFWWindowManager/GLFWWindowManagerApi.h>

#include <Core/IWindowManager.h>
#include <GLFWWindowManager/GLFWWindow.h>

namespace GLEngine::GLFWManager {

class C_GLFWWindowManager : public Core::I_WindowManager {
public:
	C_GLFWWindowManager(Core::C_Application::EventCallbackFn eventCallback, Core::E_Driver driver);
	//=============================================================
	// I_WindowManager
	//=============================================================
	[[nodiscard]] virtual std::shared_ptr<Core::I_Window> OpenNewWindow(const Core::S_WindowInfo& info) override;
	[[nodiscard]] virtual std::shared_ptr<Core::I_Window> GetWindow(GUID guid) const override;
	virtual void Update() override;
	[[nodiscard]] virtual unsigned int NumWindows() const override;
	[[nodiscard]] virtual Renderer::I_Renderer& GetActiveRenderer() override;
	[[nodiscard]] virtual Renderer::I_Renderer* ActiveRendererPtr() override;

	//=================================================================================
	// Core::C_Layer
	//=================================================================================
	virtual void OnEvent(Core::I_Event& event) override;

protected:
	virtual void Init();

private:
	std::vector<std::shared_ptr<Core::I_Window>> m_Windows;
	std::shared_ptr<Core::I_Window> m_UpdatingWindow;
	Core::E_Driver				m_Driver;
};
}