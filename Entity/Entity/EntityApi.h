#pragma once

#if !defined CORE_PLATFORM
	#error "CORE_PLATFORM not defined"
#endif

#if CORE_PLATFORM == CORE_PLATFORM_WIN
	#ifdef BUILD_ENTITY_DLL
		#define ENTITY_API_EXPORT __declspec(dllexport)
	#else
		#define ENTITY_API_EXPORT __declspec(dllimport)
	#endif
#endif

