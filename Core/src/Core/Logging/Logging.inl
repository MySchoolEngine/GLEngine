#pragma once

#include <CoreStdafx.h>

#include <Core/Logging/ILogger.h>

namespace Core {
namespace Logging {

//=================================================================================
template<class... Args>
void C_LoggingSystem::Log(Args&&... args)
{
	std::for_each(m_Loggers->begin(), m_Loggers->end(),
		[&](I_Logger* logger) {
		logger->Log(std::forward<Args>(args)...);
	});
}

}
}

