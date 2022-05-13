#include <Utils/Logging/ILogger.h>
#include <Utils/Logging/Logging.h>

namespace Utils::Logging {

//=================================================================================
C_LoggingSystem::C_LoggingSystem()
	: m_Loggers(new std::remove_pointer<decltype(m_Loggers)>::type)
{
}

//=================================================================================
C_LoggingSystem& C_LoggingSystem::Instance()
{
	static C_LoggingSystem myInstance;
	return myInstance;
}

//=================================================================================
C_LoggingSystem::~C_LoggingSystem()
{
	delete m_Loggers;
}

//=================================================================================
void C_LoggingSystem::AddLogger(I_Logger* logger)
{
	m_Loggers->emplace_back(logger);
}

//=================================================================================
void C_LoggingSystem::RemoveLogger(I_Logger* logger)
{
	m_Loggers->erase(std::remove(m_Loggers->begin(), m_Loggers->end(), logger), m_Loggers->end());
}

//=================================================================================
std::ostream& operator<<(std::ostream& out, const E_Context& c)
{
	auto enumType = rttr::type::get<E_Context>();
	auto enumContext = enumType.get_enumeration();
	out << enumContext.value_to_name(c);
	return out;
}

//=================================================================================
std::ostream& operator<<(std::ostream& out, const E_Level& c)
{
	auto enumType	 = rttr::type::get<E_Level>();
	auto enumLevel = enumType.get_enumeration();
	out << enumLevel.value_to_name(c);
	return out;
}

} // namespace Utils::Logging