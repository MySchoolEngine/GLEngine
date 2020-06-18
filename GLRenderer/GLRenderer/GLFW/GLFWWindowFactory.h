#pragma once

#include <GLRenderer/GLRendererApi.h>

#include <Core/IWindowManager.h>


namespace GLEngine::GLFWManager {
class C_GLFWWindowManager;
}

namespace GLEngine {
namespace GLRenderer {
namespace GLFW {

class C_GLFWWindowFactory final : public Core::I_WindowFactory{
public:
	C_GLFWWindowFactory() = default;
	virtual std::shared_ptr<Core::I_Window> GetWindow(const Core::S_WindowInfo&) const override;
};

GL_RENDERER_API_EXPORT Core::I_WindowFactory* ConstructGLFWWindowFactory();
GL_RENDERER_API_EXPORT GLFWManager::C_GLFWWindowManager* ConstructGLGLFWManager(Core::C_Application::EventCallbackFn eventCallback);

}
}
}