#pragma once

#include <Core/Logging/LoggingTypes.h>
#include <Core/Logging/Logging.h>

#include <Core/Utils/CompileTimeUtils.h>

#ifdef GL_ENGINE_DEBUG
	#define CORE_LOG(level, ctx, message, ...) Core::Logging::C_LoggingSystem::Instance().Log(Core::Logging::E_Level::level, Core::Logging::E_Context::ctx, __LINE__, __FILENAME__, message)
#else
	#define CORE_LOG(level, ctx, message, ...)
#endif

