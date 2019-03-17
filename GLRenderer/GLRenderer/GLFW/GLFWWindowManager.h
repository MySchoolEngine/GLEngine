#pragma once

#include <Core/CoreMacros.h>

#include <Core/IWindowManager.h>
#include <GLRenderer/GLFW/GLFWWindow.h>

#include <vector>

namespace GLEngine {
namespace GLRenderer {
namespace GLFW {

class C_GLFWWindowManager : public Core::I_WindowManager {
public:
	C_GLFWWindowManager();
	//=============================================================
	// I_WindowManager
	//=============================================================
	virtual std::shared_ptr<Core::I_Window> OpenNewWindow(const Core::S_WindowInfo& info) override;
	virtual void Update() override;
	virtual unsigned int NumWindows() const override;

protected:
	void Init();

private:
	std::vector<std::shared_ptr<Core::I_Window>> m_Windows;
};

API_EXPORT Core::I_WindowManager* ConstructGLFWManager();
}
}
}