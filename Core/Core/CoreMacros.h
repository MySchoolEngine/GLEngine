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

//======================================================
// Debug members
//======================================================
#ifdef GL_ENGINE_DEBUG
#define GLE_DEBUG_MEMBER_IMPL(debugType, releaseType, name) debugType  name
#else
#define GLE_DEBUG_MEMBER_IMPL(debugType, releaseType, name) releaseType  name
#endif

#define GLE_DEBUG_MEMBER(debugType, releaseType, memberName) \
	static_assert(std::is_assignable<debugType, releaseType>::value, "Debug type has to be assignable from releaseType"); \
	GLE_DEBUG_MEMBER_IMPL(debugType, releaseType, memberName)

#ifdef GL_ENGINE_DEBUG
#define GLE_DEBUG_MEMBER_CTOR_LIST_IMPL(debugInit, releaseInit) debugInit
#else
#define GLE_DEBUG_MEMBER_CTOR_LIST_IMPL(debugInit, releaseInit) releaseInit
#endif

#define GLE_DEBUG_MEMBER_CTOR_LIST(debugInit, releaseInit, memberName) \
 memberName GLE_DEBUG_MEMBER_CTOR_LIST_IMPL(debugInit, releaseInit)