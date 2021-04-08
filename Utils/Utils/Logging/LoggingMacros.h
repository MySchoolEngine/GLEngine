#pragma once

#include <Utils/CompileTime/CompileTimeUtils.h>
#include <Utils/Logging/Logging.h>
#include <Utils/Logging/LoggingTypes.h>

#ifdef GL_ENGINE_DEBUG
#define CORE_LOG(level, ctx, message, ...)                                     \
  ::Utils::Logging::C_LoggingSystem::Instance().Log(                           \
      ::Utils::Logging::level, ::Utils::Logging::ctx, __LINE__, __FILENAME__,  \
      message, ##__VA_ARGS__)
#else
#define CORE_LOG(level, ctx, message, ...)
#endif
