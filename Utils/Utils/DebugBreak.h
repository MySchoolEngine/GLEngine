#pragma once

#ifdef GL_ENGINE_DEBUG
	#ifdef WIN32
		#define DebugBreak() __debugbreak()
	#elif __linux__
		#include <signal.h>
		#define DebugBreak() raise(SIGTRAP)
	#endif
#else
	#define  DebugBreak()
#endif
