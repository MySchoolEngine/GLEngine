#pragma once

#include <Core/CoreMacros.h>

#include <Core/Logging/LoggingTypes.h>

#include <string>

namespace Core {
namespace Logging {

class API_EXPORT I_Logger {
public:
	virtual void Log(E_Level level, E_Context context, int line, const char* file, const std::string& text) = 0;
};

// just for test
class API_EXPORT C_CoutLogger : public I_Logger {
public:
	virtual void Log(E_Level level, E_Context context, int line, const char* file, const std::string& text) override;
};
}
}