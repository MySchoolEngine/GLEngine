#include <Utils/Logging/Logging.h>

#include <Utils/Logging/ILogger.h>

namespace Utils {
namespace Logging {

//=================================================================================
C_LoggingSystem::C_LoggingSystem()
    : m_Loggers(new std::remove_pointer<decltype(m_Loggers)>::type) {}

//=================================================================================
C_LoggingSystem &C_LoggingSystem::Instance() {
  static C_LoggingSystem myInstance;
  return myInstance;
}

//=================================================================================
C_LoggingSystem::~C_LoggingSystem() { delete m_Loggers; }

//=================================================================================
void C_LoggingSystem::AddLogger(I_Logger *logger) {
  m_Loggers->emplace_back(logger);
}

//=================================================================================
void C_LoggingSystem::RemoveLogger(I_Logger *logger) {
  m_Loggers->erase(std::remove(m_Loggers->begin(), m_Loggers->end(), logger),
                   m_Loggers->end());
}

//=================================================================================
std::ostream &operator<<(std::ostream &out, const E_Context &c) {
  const char *text = nullptr;
  switch (c) {
  default:
    text = "UnknownContext";
    break;
  case E_Context::Core:
    text = "Core";
    break;
  case E_Context::Render:
    text = "Render";
    break;
  }
  out << text;
  return out;
}

//=================================================================================
std::ostream &operator<<(std::ostream &out, const E_Level &c) {
  const char *text = nullptr;
  switch (c) {

  default:
    text = "UnknownLevel";
    break;
  case E_Level::Error:
    text = "Error";
    break;
  case E_Level::Warning:
    text = "Warning";
    break;
  case E_Level::Info:
    text = "Info";
    break;
  case E_Level::Debug:
    text = "Debug";
    break;
  }
  out << text;
  return out;
}

} // namespace Logging
} // namespace Utils