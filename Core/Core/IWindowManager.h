#pragma once

#include <Core/WindowInfo.h>

#include <memory>
#include <vector>

namespace GLEngine {
namespace Core {

class I_Window;

class I_WindowFactory {
public:
	/** ==============================================
	 * @method:    GetWindow
	 * @return:    std::shared_ptr<Core::I_Window>
	 * @param: 	   const S_WindowInfo &
	 * @brief	   Creates new instance of I_Window, or null if this factory
	 *			   doesn't handle this type of window.
	 ** ==============================================*/
	virtual std::shared_ptr<I_Window> GetWindow(const S_WindowInfo&) const = 0;
};

class API_EXPORT I_WindowManager {
public:
	I_WindowManager();
	virtual ~I_WindowManager();
	virtual std::shared_ptr<I_Window> OpenNewWindow(const S_WindowInfo& info) = 0;
	virtual void AddWindowFactory(I_WindowFactory* wf);
	virtual void Update() = 0;
	virtual unsigned int NumWindows() const = 0;
protected:
	std::shared_ptr<I_Window> ConstructWindow(const S_WindowInfo& info) const;
private:
	std::vector<I_WindowFactory*>* m_Facotries;
};
}}