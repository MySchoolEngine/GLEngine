#pragma once

#ifdef GL_ENGINE_DEBUG
	#ifdef WIN32
		#define GL_DebugBreak() __debugbreak()
	#elif __linux__
		#include <signal.h>
		#define GL_DebugBreak() raise(SIGTRAP)
	#endif
#else
	#define  GL_DebugBreak()
#endif
