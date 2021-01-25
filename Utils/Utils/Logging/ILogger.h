#pragma once

#include <Utils/Logging/LoggingTypes.h>
#include <Utils/UtilsApi.h>

#include <filesystem>
#include <fstream>
#include <string>

namespace Utils::Logging {

class UTILS_API_EXPORT I_Logger {
public:
	virtual void Log(E_Level level, E_Context context, int line, const char* file, const std::string& text) = 0;
};

// just for test
class UTILS_API_EXPORT C_CoutLogger : public I_Logger {
public:
	virtual void Log(E_Level level, E_Context context, int line, const char* file, const std::string& text) override;
};

class UTILS_API_EXPORT C_FileLogger : public I_Logger {
public:
	explicit C_FileLogger(const std::filesystem::path& filename);
	virtual ~C_FileLogger();
	virtual void Log(E_Level level, E_Context context, int line, const char* file, const std::string& text) override;

protected:
	std::ofstream* m_file;
};
} // namespace Utils::Logging