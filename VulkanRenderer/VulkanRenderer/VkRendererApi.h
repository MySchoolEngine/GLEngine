#pragma once

#if !defined CORE_PLATFORM
#error "CORE_PLATFORM not defined"
#endif

#if CORE_PLATFORM == CORE_PLATFORM_WIN
	#ifdef BUILD_VULKAN_RENDERER_DLL
		#define VK_RENDERER_API_EXPORT __declspec(dllexport)
	#else
		#define VK_RENDERER_API_EXPORT __declspec(dllimport)
	#endif
#endif

