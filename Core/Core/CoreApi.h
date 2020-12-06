#pragma once

#if !defined CORE_PLATFORM
	#error "CORE_PLATFORM not defined"
#endif

#ifdef WIN32
	#ifdef BUILD_CORE_DLL
		#define CORE_API_EXPORT __declspec(dllexport)
	#else
		#define CORE_API_EXPORT __declspec(dllimport)
	#endif
#elif __linux__
	#ifdef BUILD_CORE_DLL
		#define CORE_API_EXPORT
	#else
		#define CORE_API_EXPORT
	#endif
#endif

