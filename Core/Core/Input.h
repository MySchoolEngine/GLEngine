#pragma once

#include <Core/CoreMacros.h>

#include <utility>

namespace GLEngine {
namespace Core {

class API_EXPORT I_Input {
public:
	virtual ~I_Input() = default;

	virtual bool IsKeyPressed(int keycode) const = 0;
	virtual bool IsMosueButtonPresse(int key) const = 0;
	virtual std::pair<float, float> GetMousePosition() const = 0;
	virtual float GetMouseX() const = 0;
	virtual float GetMouseY() const = 0;
};

}
}