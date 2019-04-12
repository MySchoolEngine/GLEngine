#pragma once

#include <CoreStdafx.h>

#include <Core/EventSystem/Event/InputEvents.h>

namespace GLEngine {
namespace Core {

class API_EXPORT C_MouseScrollEvent : public C_InputEvent {
public:
	C_MouseScrollEvent(float dx, float dy, GUID window)
		: C_InputEvent(window), dx(dx), dy(dy) {}

	inline float GetXOffset() const { return dx; }
	inline float GetYOffset() const { return dy; }

	virtual Utils::C_BitField<E_EventCategory> GetInputCategory() const override
	{
		return E_EventCategory::Mouse;
	}
	EVENT_CLASS_TYPE(MouseScroll);
private:
	float dx, dy;
};

}}