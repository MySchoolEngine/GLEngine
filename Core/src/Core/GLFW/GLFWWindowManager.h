#pragma once

#include <Core/CoreMacros.h>

#include <Core/IWindowManager.h>
#include <Core/GLFW/GLFWWindow.h>

#include <vector>

namespace Core {
class C_GLFWWindowManager : public I_WindowManager {
public:
	API_EXPORT C_GLFWWindowManager();
	//=============================================================
	// I_WindowManager
	//=============================================================
	virtual std::shared_ptr<I_Window> OpenNewWindow(const S_WindowInfo& info) override;
	virtual void Update() override;
	virtual unsigned int NumWindows() const override;

protected:
	void Init();

private:
	std::vector<std::shared_ptr<C_GLFWWindow>> m_Windows;
};
}