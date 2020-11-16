#pragma once

#if !defined CORE_PLATFORM
	#error "CORE_PLATFORM not defined"
#endif

#if WIN32
	#ifdef BUILD_PHYSICS_DLL
		#define PHYSICS_API_EXPORT __declspec(dllexport)
	#else
		#define PHYSICS_API_EXPORT __declspec(dllimport)
	#endif
#elif __linux__
	#ifdef BUILD_PHYSICS_DLL
		#define PHYSICS_API_EXPORT [[gnu::dllexport]]
	#else
		#define PHYSICS_API_EXPORT [[gnu::dllimport]]
	#endif
#endif

