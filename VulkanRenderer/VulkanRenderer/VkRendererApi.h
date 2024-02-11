#pragma once

#if !defined CORE_PLATFORM
	#error "CORE_PLATFORM not defined"
#endif

#if WIN32
	#ifdef BUILD_VULKANRENDERER_DLL
		#define VK_RENDERER_API_EXPORT __declspec(dllexport)
	#else
		#define VK_RENDERER_API_EXPORT __declspec(dllimport)
	#endif
#elif __linux__
	#ifdef BUILD_VULKANRENDERER_DLL
		#define VK_RENDERER_API_EXPORT
	#else
		#define VK_RENDERER_API_EXPORT
	#endif
#endif
