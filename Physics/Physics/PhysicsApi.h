#pragma once

#if !defined CORE_PLATFORM
	#error "CORE_PLATFORM not defined"
#endif

#if CORE_PLATFORM == CORE_PLATFORM_WIN
	#ifdef BUILD_PHYSICS_DLL
		#define PHYSICS_API_EXPORT __declspec(dllexport)
	#else
		#define PHYSICS_API_EXPORT __declspec(dllimport)
	#endif
#endif

