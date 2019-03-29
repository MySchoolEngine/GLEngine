#pragma once

#if CORE_PLATFORM == CORE_PLATFORM_WIN

extern Core::C_Application* Core::CreateApplication();

int main(int argc, char** argv)
{
	CORE_LOG(E_Level::Info, E_Context::Core, "Initialized");

	auto app = Core::CreateApplication();
	app->LoadArgs(argc, argv);
	app->Init();
	app->Run();
	CORE_LOG(E_Level::Info, E_Context::Core, "App ended");
	delete app;
}

#endif