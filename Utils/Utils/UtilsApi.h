#pragma once

#if !defined CORE_PLATFORM
	#error "CORE_PLATFORM not defined"
#endif

#if CORE_PLATFORM == CORE_PLATFORM_WIN
	#ifdef BUILD_UTILS_DLL
		#define UTILS_API_EXPORT __declspec(dllexport)
	#else
		#define UTILS_API_EXPORT __declspec(dllimport)
	#endif
#endif

