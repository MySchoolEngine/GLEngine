#pragma once

#if !defined CORE_PLATFORM
	#error "CORE_PLATFORM not defined"
#endif

#if _WIN32
	#ifdef BUILD_GUI_DLL
		#define GUI_API_EXPORT __declspec(dllexport)
	#else
		#define GUI_API_EXPORT __declspec(dllimport)
	#endif
#elif __linux__
	#ifdef BUILD_GUI_DLL
		#define GUI_API_EXPORT [[gnu::dllexport]]
	#else
		#define GUI_API_EXPORT [[gnu::dllimport]]
	#endif
#endif

