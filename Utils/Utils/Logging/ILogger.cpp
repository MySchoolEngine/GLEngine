#include <Utils/Logging/ILogger.h>
#include <Utils/Logging/Logging.h>

#include <iostream>

namespace Utils { namespace Logging {

//=================================================================================
void C_CoutLogger::Log(E_Level level, E_Context context, int line, const char* file, const std::string& text)
{
	std::cout << file << ":" << line << "(" << level << "," << context << "): " << text << "\n";
}

//=================================================================================
C_FileLogger::C_FileLogger(const std::filesystem::path& filename)
	: m_file(new std::remove_pointer<decltype(m_file)>::type)
{
	m_file->open(filename);
}

//=================================================================================
C_FileLogger::~C_FileLogger()
{
	m_file->close();
	delete m_file;
}

//=================================================================================
void C_FileLogger::Log(E_Level level, E_Context context, int line, const char* file, const std::string& text)
{
	(*m_file) << file << ":" << line << "(" << level << "," << context << "): " << text << "\n";
}

}} // namespace Utils::Logging
