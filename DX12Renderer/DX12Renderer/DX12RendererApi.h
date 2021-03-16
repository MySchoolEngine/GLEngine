#pragma once

#if !defined CORE_PLATFORM
	#error "CORE_PLATFORM not defined"
#endif

#if WIN32
	#ifdef BUILD_DX12RENDERER_DLL
		#define DX12_RENDERER_API_EXPORT __declspec(dllexport)
	#else
		#define DX12_RENDERER_API_EXPORT __declspec(dllimport)
	#endif
#elif __linux__
	#ifdef BUILD_DX12RENDERER_DLL
		#define DX12_RENDERER_API_EXPORT [[gnu::dllexport]]
	#else
		#define DX12_RENDERER_API_EXPORT [[gnu::dllimport]]
	#endif
#endif
