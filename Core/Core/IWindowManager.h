#pragma once

#include <Core/EventSystem/Layer.h>
#include <Core/Application.h>

namespace GLEngine {
namespace Core {

struct S_WindowInfo;

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

//=================================================================================
class API_EXPORT I_WindowManager : public C_Layer {
public:
	I_WindowManager(C_Application::EventCallbackFn callback);
	virtual ~I_WindowManager();
	virtual std::shared_ptr<I_Window> OpenNewWindow(const S_WindowInfo& info) = 0;
	virtual std::shared_ptr<I_Window> GetWindow(GUID guid) const = 0;
	virtual void AddWindowFactory(I_WindowFactory* wf);
	virtual void Update() = 0;
	virtual unsigned int NumWindows() const = 0;
	virtual const std::unique_ptr<GLEngine::Renderer::I_Renderer>& GetActiveRenderer() const = 0;
protected:
	std::shared_ptr<I_Window> ConstructWindow(const S_WindowInfo& info) const;

#pragma warning(push)
#pragma warning( disable : 4251)
	Core::C_Application::EventCallbackFn m_EventCallback;
#pragma warning(pop)
private:
	std::vector<I_WindowFactory*>* m_Facotries;
};
}}