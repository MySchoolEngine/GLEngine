#pragma once

#include <Core/CoreEnums.h>

namespace GLEngine::Core {

//=================================================================================
struct S_WindowInfo {
	S_WindowInfo(unsigned int width, unsigned int height)
		: m_width(width)
		, m_height(height)
		, m_Fullscreen(false)
		, m_Maximize(true)
	{}
	S_WindowInfo(const S_WindowInfo& other)		= default;
	S_WindowInfo(S_WindowInfo&& other) noexcept = default;
	S_WindowInfo& operator=(const S_WindowInfo& other) = default;
	S_WindowInfo& operator=(S_WindowInfo&& other) noexcept = default;
	virtual ~S_WindowInfo() = default;

	std::string	 m_name;
	unsigned int m_width, m_height;
	std::string	 m_WindowClass;
	bool		 m_Fullscreen : 1;
	bool		 m_Maximize : 1;

	virtual E_Driver GetDriver() const = 0;
};
} // namespace GLEngine::Core
