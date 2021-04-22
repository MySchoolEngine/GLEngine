#pragma once

#include <Core/CoreApi.h>

namespace GLEngine::Core {

class CORE_API_EXPORT I_Input {
public:
	virtual ~I_Input();

	[[nodiscard]] virtual bool					  IsKeyPressed(int keycode) const	 = 0;
	[[nodiscard]] virtual bool					  IsMosueButtonPresse(int key) const = 0;
	[[nodiscard]] virtual std::pair<float, float> GetMousePosition() const			 = 0;
	[[nodiscard]] virtual float					  GetMouseX() const					 = 0;
	[[nodiscard]] virtual float					  GetMouseY() const					 = 0;
};

} // namespace GLEngine::Core