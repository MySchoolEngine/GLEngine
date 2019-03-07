#pragma once

#include <CoreStdafx.h>

#include <Core/Logging/ILogger.h>

#include <fmt/format.h>

namespace Core {
namespace Logging {

//=================================================================================
template<class... Args>
void C_LoggingSystem::Log(E_Level level, E_Context context, int line, const char* file, const char* format, Args&&... args)
{
	auto msg = fmt::format(format, std::forward<Args>(args)...);
	std::for_each(m_Loggers->begin(), m_Loggers->end(),
		[&](I_Logger* logger) {
		logger->Log(level, context, line, file, msg);
	});
}

}
}

