#pragma once

#include <Core/CoreMacros.h>

#include <Core/WindowInfo.h>
#include <Core/EventSystem/Layer.h>
#include <Core/Application.h>

#include <glm/vec2.hpp>

namespace GLEngine {

namespace Renderer {
class I_Renderer;
}


namespace Core {
/************************************************************************/
/* Represents single window on screen                                   */
/************************************************************************/
class API_EXPORT I_Window : public C_Layer {
public:
	virtual ~I_Window() = default;
	virtual unsigned int GetWidth() const = 0;
	virtual unsigned int GetHeight() const = 0;
	virtual glm::uvec2	 GetSize() const = 0;

	virtual void Update() = 0;

	/** ==============================================
	 * @method:    Init
	 * @fullName:  Core::I_Window::Init
	 * @return:    void
	 * @param: 	   const S_WindowInfo & - should be corresponding subcalss
	 * @brief
	 ** ==============================================*/
	virtual void Init(const S_WindowInfo& wndInfo) = 0;
	virtual void SetTitle(const std::string& title) = 0;
	virtual void SetEventCallback(C_Application::EventCallbackFn callback) = 0;

	virtual bool WantClose() const = 0;

	virtual const std::unique_ptr<GLEngine::Renderer::I_Renderer>& GetRenderer() const = 0;

	inline GUID GetGUID() const { return m_ID; }
protected:
	virtual void Destroy() = 0;
	I_Window()
		: C_Layer("window")
		, m_ID(NextGUID()) {}
	GUID m_ID;
};
}}