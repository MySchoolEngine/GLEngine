#pragma once

#include <Core/CoreApi.h>

namespace GLEngine::Core {

class CORE_API_EXPORT I_Input {
public:
	virtual ~I_Input();

	enum class E_MouseCursor : std::uint8_t{
		Arrow,
		Hand,
		TextInput,
		NSResize,
		WEResize,
		Crosshair,
		Count,
	};

	[[nodiscard]] virtual bool					  IsKeyPressed(int keycode) const	  = 0;
	[[nodiscard]] virtual bool					  IsMouseButtonPressed(int key) const = 0;
	[[nodiscard]] virtual std::pair<float, float> GetMousePosition() const			  = 0;
	[[nodiscard]] virtual glm::vec2				  GetClipSpaceMouseCoord() const	  = 0;
	[[nodiscard]] virtual float					  GetMouseX() const					  = 0;
	[[nodiscard]] virtual float					  GetMouseY() const					  = 0;

	virtual void SetMouseCursor(E_MouseCursor cursor) = 0;
};

} // namespace GLEngine::Core