#pragma once

#include <Core/CoreApi.h>
#include <Core/CoreMacros.h>

#include <utility>

namespace GLEngine::Core {

class CORE_API_EXPORT I_Input {
public:
	virtual ~I_Input();

	virtual bool					IsKeyPressed(int keycode) const	   = 0;
	virtual bool					IsMosueButtonPresse(int key) const = 0;
	virtual std::pair<float, float> GetMousePosition() const		   = 0;
	virtual float					GetMouseX() const				   = 0;
	virtual float					GetMouseY() const				   = 0;
};

} // namespace GLEngine::Core