#pragma once

#include <Core/CoreMacros.h>

#include <Core/Logging/LoggingTypes.h>

#include <string>
#include <fstream>

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

class API_EXPORT C_FileLogger : public I_Logger {
public:
	C_FileLogger(const std::string& filename);
	virtual ~C_FileLogger();
	virtual void Log(E_Level level, E_Context context, int line, const char* file, const std::string& text) override;

protected:
	std::ofstream* m_file;
};
}
}