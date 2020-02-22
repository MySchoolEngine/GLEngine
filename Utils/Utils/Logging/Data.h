#pragma once

#include <Utils/Logging/Logging.h>

#include <string>

namespace Utils {
namespace Logging {

struct UTILS_API_EXPORT S_Data {

#pragma warning(push)
#pragma warning( disable : 4251)
	std::string m_Text;
	std::string m_File;
	int			m_Line;
	E_Level		m_Level;
	E_Context	m_Context;
#pragma warning(pop)
};
}
}

