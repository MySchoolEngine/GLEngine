#pragma once

#include <Core/EventSystem/Event/AppEvent.h>

#if CORE_PLATFORM == CORE_PLATFORM_WIN

	#ifdef GL_ENGINE_DEBUG
		#include <fcntl.h>
		#include <io.h>
		#include <stdio.h>
		#include <windows.h>
	#endif

extern GLEngine::Core::C_Application* GLEngine::Core::CreateApplication();

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	#ifdef GL_ENGINE_DEBUG
	AllocConsole();
	const auto err = freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	if (err != 0)
	{
		FreeConsole();
		exit(-1);
	}
	#endif


	CORE_LOG(E_Level::Info, E_Context::Core, "Initialized");

	auto app = GLEngine::Core::CreateApplication();
	app->LoadArgs(0, nullptr);
	app->Init(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	GLEngine::Core::C_AppEvent event(GLEngine::Core::C_AppEvent::E_Type::AppInit);
	app->OnEvent(event);
	app->Run();
	CORE_LOG(E_Level::Info, E_Context::Core, "App ended");
	delete app;
	#ifdef GL_ENGINE_DEBUG
	FreeConsole();
	#endif
}

#else
extern GLEngine::Core::C_Application* GLEngine::Core::CreateApplication();

int main(int argc, char** argv)
{
	CORE_LOG(E_Level::Info, E_Context::Core, "Initialized");

	auto app = GLEngine::Core::CreateApplication();
	app->LoadArgs(argc, argv);
	app->Init();
	GLEngine::Core::C_AppEvent event(GLEngine::Core::C_AppEvent::E_Type::AppInit);
	app->OnEvent(event);
	app->Run();
	CORE_LOG(E_Level::Info, E_Context::Core, "App ended");
	delete app;
}


#endif