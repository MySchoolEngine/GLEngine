#pragma once

#include <Core/CoreMacros.h>

#include <Core/IWindowManager.h>

namespace GLEngine {
namespace GLRenderer {
namespace GLFW {

class C_GLFWWindowFactory final : public Core::I_WindowFactory{
public:
	C_GLFWWindowFactory() = default;
	virtual std::shared_ptr<Core::I_Window> GetWindow(const Core::S_WindowInfo&) const override;
};

API_EXPORT Core::I_WindowFactory* ConstructGLFWWindowFactory();
}
}
}