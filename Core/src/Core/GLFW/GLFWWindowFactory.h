#pragma once

#include <Core/IWindowManager.h>

namespace Core {
namespace GLFW {

class C_GLFWWindowFactory : public I_WindowFactory {
public:
	virtual std::shared_ptr<I_Window> GetWindow(const S_WindowInfo&) const override;

};
}
}