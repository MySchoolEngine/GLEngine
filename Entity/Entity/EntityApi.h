#pragma once

#if !defined CORE_PLATFORM
	#error "CORE_PLATFORM not defined"
#endif

#if WIN32
	#ifdef BUILD_ENTITY_DLL
		#define ENTITY_API_EXPORT __declspec(dllexport)
	#else
		#define ENTITY_API_EXPORT __declspec(dllimport)
	#endif
#elif __linux__
	#ifdef BUILD_ENTITY_DLL
		#define ENTITY_API_EXPORT
	#else
		#define ENTITY_API_EXPORT
	#endif
#endif
