#include <iostream>

#include <Core/CoreMacros.h>

#include <Core/GLFW/GLFWWindow.h>
#include <Core/GLFW/GLFWWindowManager.h>
#include <Core/Logging/Logging.h>
#include <Core/Logging/ILogger.h>

int main(int args, char** argv) {
	auto& logging = Core::Logging::C_LoggingSystem::Instance();
	logging.AddLogger(new Core::Logging::C_CoutLogger());
	//logging.AddLogger(new Core::Logging::C_FileLogger("log.txt"));

	Core::S_WindowInfo info;
	info.m_name = "My title";

	Core::I_WindowManager* wmng = new Core::GLFW::C_GLFWWindowManager();
	wmng->OpenNewWindow(info);

	while (wmng->NumWindows()) {
		wmng->Update();
	}

	CORE_LOG(E_Level::Info, E_Context::Core, "App ended");

	return 0;
}