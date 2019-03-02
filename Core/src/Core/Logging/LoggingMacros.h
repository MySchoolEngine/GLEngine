#pragma once

#include <Core/Logging/LoggingTypes.h>
#include <Core/Logging/Logging.h>

#include <Core/Utils/CompileTimeUtils.h>

#ifdef GL_ENGINE_DEBUG
	#define CORE_LOG(level, ctx, message, ...) Core::Logging::C_LoggingSystem::Instance().Log(Core::Logging::level, Core::Logging::ctx, __LINE__, __FILENAME__, message, __VA_ARGS__)
#else
	#define CORE_LOG(level, ctx, message, ...)
#endif

