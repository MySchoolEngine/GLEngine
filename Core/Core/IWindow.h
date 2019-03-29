#pragma once

#include <Core/CoreMacros.h>

#include <Core/WindowInfo.h>

#include <glm/vec2.hpp>

#include <string>

namespace GLEngine {
namespace Core {
/************************************************************************/
/* Represents single window on screen                                   */
/************************************************************************/
class I_Window {
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
protected:
	virtual void Destroy() = 0;
};
}}