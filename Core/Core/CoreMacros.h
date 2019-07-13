#pragma once

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

#define BIT(x) (1 << x)

//======================================================
// Asserts
//======================================================
#ifdef GL_ENGINE_DEBUG
#define GLE_ASSERT(expr, message, ...) if(!(expr)) { CORE_LOG(E_Level::Error, E_Context::Core, "Assert failed: " message, ##__VA_ARGS__); __debugbreak();}
#else 
#define GLE_ASSERT(expr, message, ...) if(!(expr)) {CORE_LOG(E_Level::Error, E_Context::Core, "Assert failed: {}" message, ##__VA_ARGS__);}
#endif