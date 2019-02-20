#pragma once

#include <string>
#include <memory>

namespace Core {

class I_Window;

struct S_WindowInfo {
	std::string m_name;
	unsigned int m_width, m_height;
};

class API_EXPORT I_WindowManager {
public:
	virtual std::shared_ptr<I_Window> OpenNewWindow(const S_WindowInfo& info) = 0;
	virtual void Update() = 0;
};
}