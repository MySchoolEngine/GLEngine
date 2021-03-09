#pragma once

#if !defined CORE_PLATFORM
	#error "CORE_PLATFORM not defined"
#endif

#if WIN32
	#ifdef BUILD_RENDERER_DLL
		#define RENDERER_API_EXPORT __declspec(dllexport)
	#else
		#define RENDERER_API_EXPORT __declspec(dllimport)
	#endif
	#define RENDERER_TEMPLATE_EXPORT
#elif __linux__
	#ifdef BUILD_RENDERER_DLL
		#define RENDERER_API_EXPORT
	#else
		#define RENDERER_API_EXPORT
	#endif
	#define RENDERER_TEMPLATE_EXPORT extern
#endif

