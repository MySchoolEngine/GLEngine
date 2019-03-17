#pragma once

#include <GLRenderer/GLFW/GLFWoGLWindow.h>

namespace GLEngine {
namespace GLRenderer {
namespace Windows {
class C_ExplerimentWindow : public GLFW::C_GLFWoGLWindow {
public:
	C_ExplerimentWindow(const Core::S_WindowInfo& wndInfo);
	//=================================================================================
	virtual void Update() override;

};
}
}
}