#pragma once

#include <Core/Utils/CompileTimeUtils.h>

#ifdef GL_ENGINE_DEBUG
	#define CORE_LOG(level, ctx, message, ...) Core::Logging::C_LoggingSystem::Instance().Log(level, ctx, __LINE__, __FILENAME__, message)
#else
	#define CORE_LOG(level, ctx, message, ...)
#endif

