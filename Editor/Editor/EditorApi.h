#pragma once

#if !defined CORE_PLATFORM
	#error "CORE_PLATFORM not defined"
#endif

#if WIN32
	#ifdef BUILD_EDITOR_DLL
		#define EDITOR_API_EXPORT __declspec(dllexport)
	#else
		#define EDITOR_API_EXPORT __declspec(dllimport)
	#endif
#elif __linux__
	#ifdef BUILD_EDITOR_DLL
		#define EDITOR_API_EXPORT
	#else
		#define EDITOR_API_EXPORT
	#endif
#endif
