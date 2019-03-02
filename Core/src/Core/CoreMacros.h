#pragma once

#include <Core/Logging/LoggingMacros.h>

#if !defined CORE_PLATFORM
	#error "CORE_PLATFORM not defined"
#endif


#if CORE_PLATFORM == CORE_PLATFORM_WIN
	#ifdef BUILD_DLL
		#define API_EXPORT __declspec(dllexport)
	#else
		#define API_EXPORT __declspec(dllimport)
	#endif
#endif