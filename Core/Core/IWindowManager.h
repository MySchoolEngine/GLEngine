#pragma once

#include <Core/Application.h>
#include <Core/CoreApi.h>
#include <Core/EventSystem/Layer.h>

namespace GLEngine::Core {

struct S_WindowInfo;

class I_Window;

class I_WindowFactory {
public:
	virtual ~I_WindowFactory() = default;
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
class I_WindowManager : public C_Layer {
public:
	CORE_API_EXPORT I_WindowManager(const C_Application::EventCallbackFn& callback);
	CORE_API_EXPORT ~I_WindowManager() override;
	virtual std::shared_ptr<I_Window> OpenNewWindow(const S_WindowInfo& info) = 0;
	virtual std::shared_ptr<I_Window> GetWindow(GUID guid) const			  = 0;
	CORE_API_EXPORT virtual void	  AddWindowFactory(I_WindowFactory* wf);
	virtual void					  Update()			  = 0;
	virtual unsigned int			  NumWindows() const  = 0;
	virtual Renderer::I_Renderer&	  GetActiveRenderer() = 0;
	// this function is meant for functions unsure from which part of frame are being called
	[[nodiscard]] virtual Renderer::I_Renderer* ActiveRendererPtr() = 0;

protected:
	CORE_API_EXPORT std::shared_ptr<I_Window> ConstructWindow(const S_WindowInfo& info) const;

	C_Application::EventCallbackFn m_EventCallback;

private:
	std::vector<I_WindowFactory*>* m_Factories;
};
} // namespace GLEngine::Core