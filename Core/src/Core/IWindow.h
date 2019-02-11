#pragma once

#include <Core/CoreMacros.h>

#include <string>

/************************************************************************/
/* Represents single window on screen                                   */
/************************************************************************/
class API_EXPORT I_Window {
public:
	virtual ~I_Window() = default;

	virtual unsigned int GetWidth() const = 0;
	virtual unsigned int GetHeight() const = 0;

	virtual void SetTitle(std::string& title) = 0;
};