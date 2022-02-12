#pragma once

#include <Utils/UtilsApi.h>

#include <iostream>
#include <string>
#include <vector>

namespace Utils::Logging {

class I_Logger;
enum class E_Context;
enum class E_Level;

//=================================================================================
std::ostream& operator<<(std::ostream& out, const E_Context& c);
std::ostream& operator<<(std::ostream& out, const E_Level& c);

//=================================================================================
class UTILS_API_EXPORT C_LoggingSystem final {
public:
	static C_LoggingSystem& Instance();

	// delete copy and move constructors and assign operators
	C_LoggingSystem(C_LoggingSystem const&) = delete;			 // Copy construct
	C_LoggingSystem(C_LoggingSystem&&)		= delete;			 // Move construct
	C_LoggingSystem& operator=(C_LoggingSystem const&) = delete; // Copy assign
	C_LoggingSystem& operator=(C_LoggingSystem&&) = delete;		 // Move assign

	template <class... Args> void Log(E_Level level, E_Context context, int line, const char* file, const char* fmt, Args&&... args);

	void AddLogger(I_Logger* logger);
	void RemoveLogger(I_Logger* logger);

private:
	C_LoggingSystem();
	~C_LoggingSystem();

	std::vector<I_Logger*>* m_Loggers;
};

} // namespace Utils::Logging

#include <Utils/Logging/Logging.inl>