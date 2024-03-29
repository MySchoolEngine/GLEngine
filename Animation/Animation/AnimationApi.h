#pragma once

#if !defined CORE_PLATFORM
	#error "CORE_PLATFORM not defined"
#endif

#if WIN32
	#ifdef BUILD_ANIMATION_DLL
		#define ANIMATION_API_EXPORT __declspec(dllexport)
	#else
		#define ANIMATION_API_EXPORT __declspec(dllimport)
	#endif
#elif __linux__
	#ifdef BUILD_ANIMATION_DLL
		#define ANIMATION_API_EXPORT
	#else
		#define ANIMATION_API_EXPORT
	#endif
#endif
