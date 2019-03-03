#pragma once

#include <Core/WindowInfo.h>

#include <memory>

namespace Core {

class I_Window;

class I_WindowFactory {
public:
	virtual ~I_WindowFactory() = default;
	virtual std::shared_ptr<I_Window> GetWindow(const S_WindowInfo&) const = 0;
};

class API_EXPORT I_WindowManager {
public:
	virtual std::shared_ptr<I_Window> OpenNewWindow(const S_WindowInfo& info) = 0;
	virtual void Update() = 0;
	virtual unsigned int NumWindows() const = 0;
protected:
	virtual std::shared_ptr<I_WindowFactory> GetWindowFactory() const = 0;
};
}