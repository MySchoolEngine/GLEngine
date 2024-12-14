#pragma once

#include <Utils/Logging/Formatters/GLMFormatters.h>
#include <Utils/Logging/Formatters/filepath.h>
#include <Utils/Logging/Formatters/rttr.h>
#include <Utils/Logging/ILogger.h>

#include <fmt/format.h>

namespace Utils::Logging {

//=================================================================================
template <class... Args> void C_LoggingSystem::Log(E_Level level, E_Context context, int line, const char* file, const char* fmt, Args&&... args)
{
	auto msg = fmt::format(fmt, std::forward<Args>(args)...);
	std::for_each(m_Loggers->begin(), m_Loggers->end(), [&](I_Logger* logger) { logger->Log(level, context, line, file, msg); });
}

} // namespace Utils::Logging
