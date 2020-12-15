#pragma once

#if !defined CORE_PLATFORM
	#error "CORE_PLATFORM not defined"
#endif

#ifdef WIN32
	#ifdef BUILD_GLFWWINDOWMANAGER_DLL
		#define GL_GLFWWINDOWMANAGER_API_EXPORT __declspec(dllexport)
	#else
		#define GL_GLFWWINDOWMANAGER_API_EXPORT __declspec(dllimport)
	#endif
#elif __linux__
	#ifdef BUILD_GLFWWINDOWMANAGER_DLL
		#define GL_GLFWWINDOWMANAGER_API_EXPORT
	#else
		#define GL_GLFWWINDOWMANAGER_API_EXPORT
	#endif
#endif
