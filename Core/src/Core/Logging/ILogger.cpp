#include <CoreStdafx.h>

#include <Core/Logging/ILogger.h>
#include <Core/Logging/Logging.h>

#include <iostream>

namespace Core {
namespace Logging {

//=================================================================================
void C_CoutLogger::Log(E_Level level, E_Context context, int line, const char* file, const std::string& text)
{
	std::cout << file << ":" << line << "(" << level << "," << context << "): " << text;
}

}
}
