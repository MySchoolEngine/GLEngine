#pragma once

#define BIT(x) (1 << x)

//======================================================
// Asserts
//======================================================
#ifdef GL_ENGINE_DEBUG
#define GLE_ASSERT(expr, message, ...) if(!(expr)) { CORE_LOG(E_Level::Error, E_Context::Core, "Assert failed: " message, ##__VA_ARGS__); __debugbreak();}
#else 
#define GLE_ASSERT(expr, message, ...) if(!(expr)) {CORE_LOG(E_Level::Error, E_Context::Core, "Assert failed: {}" message, ##__VA_ARGS__);}
#endif