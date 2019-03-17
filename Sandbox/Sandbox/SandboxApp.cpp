#include <iostream>

#include <Core/CoreMacros.h>

#include <GLRenderer/GLFW/GLFWWindow.h>
#include <GLRenderer/GLFW/GLFWWindowManager.h>
#include <GLRenderer/GLFW/GLFWWindowFactory.h>

#include <Utils/Logging/Logging.h>
#include <Utils/Logging/ILogger.h>
#include <Utils/Logging/LoggingMacros.h>

int main(int args, char** argv) {
	auto& logging = Utils::Logging::C_LoggingSystem::Instance();
	logging.AddLogger(new Utils::Logging::C_CoutLogger());
	//logging.AddLogger(new Core::Logging::C_FileLogger("log.txt"));

	Core::S_OpenGLWindowInfo info(640, 480);
	info.m_name = "My title";
	info.m_WindowClass = "GLFW_OGL";
	info.m_MinorVersion = 0;

	Core::I_WindowManager* wmng = GLEngine::GLRenderer::GLFW::ConstructGLFWManager();
	wmng->AddWindowFactory(GLEngine::GLRenderer::GLFW::ConstructGLFWWindowFactory());
	wmng->OpenNewWindow(info);

	while (wmng->NumWindows()) {
		wmng->Update();
	}

	CORE_LOG(E_Level::Info, E_Context::Core, "App ended");

	return 0;
}