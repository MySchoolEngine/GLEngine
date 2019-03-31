#pragma once

#include <Core/CoreMacros.h>

#include <Core/WindowInfo.h>
#include <Core/EventSystem/Event.h>

#include <glm/vec2.hpp>

#include <string>
#include <memory>

namespace GLEngine {

namespace Renderer {
class I_Renderer;
}


namespace Core {
/************************************************************************/
/* Represents single window on screen                                   */
/************************************************************************/
class API_EXPORT I_Window{
public:
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

	virtual bool WantClose() const = 0;

	virtual const std::unique_ptr<GLEngine::Renderer::I_Renderer>& GetRenderer() const = 0;

	inline GUID GetGUID() const { return m_ID; }

	//================================================
	// Events - no actuall need to be C_Layer
	//================================================
	virtual void OnEvent(Core::I_Event& event) = 0;
protected:
	virtual void Destroy() = 0;
	I_Window()
		:m_ID(NextGUID()) {}
	GUID m_ID;
};
}}