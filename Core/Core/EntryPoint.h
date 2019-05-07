#pragma once

#if CORE_PLATFORM == CORE_PLATFORM_WIN

#include <Core/EventSystem/Event/AppEvent.h>

extern GLEngine::Core::C_Application* GLEngine::Core::CreateApplication();

int main(int argc, char** argv)
{
	CORE_LOG(E_Level::Info, E_Context::Core, "Initialized");

	auto app = GLEngine::Core::CreateApplication();
	app->LoadArgs(argc, argv);
	app->Init();
	app->OnEvent(GLEngine::Core::C_AppEvent());
	app->Run();
	CORE_LOG(E_Level::Info, E_Context::Core, "App ended");
	delete app;
}

#endif