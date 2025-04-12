#pragma once

#if !defined CORE_PLATFORM
	#error "CORE_PLATFORM not defined"
#endif

#ifdef WIN32
	#ifdef BUILD_AI_DLL
		#define AI_API_EXPORT __declspec(dllexport)
	#else
		#define AI_API_EXPORT __declspec(dllimport)
	#endif
#elif __linux__
	#ifdef BUILD_CORE_DLL
		#define AI_API_EXPORT 
	#else
		#define AI_API_EXPORT 
	#endif
#endif
