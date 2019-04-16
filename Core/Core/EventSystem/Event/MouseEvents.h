#pragma once

#include <CoreStdafx.h>

#include <Core/EventSystem/Event/InputEvents.h>

namespace GLEngine {
namespace Core {

//=============================================================
class C_MouseScrollEvent : public C_InputEvent {
public:
	C_MouseScrollEvent(double dx, double dy, GUID window)
		: C_InputEvent(window), dx(dx), dy(dy) {}

	inline double GetXOffset() const { return dx; }
	inline double GetYOffset() const { return dy; }

	virtual Utils::C_BitField<E_EventCategory> GetInputCategory() const override
	{
		return E_EventCategory::Mouse;
	}
	EVENT_CLASS_TYPE(MouseScroll);
private:
	double dx, dy;
};

//=============================================================
class C_MouseButtonEvent : public C_InputEvent {
public:
	int GetMouseButton() const { return m_button; }

	virtual Utils::C_BitField<E_EventCategory> GetInputCategory() const override
	{
		return E_EventCategory::Mouse | E_EventCategory::MouseButton;
	}
protected:
	C_MouseButtonEvent(int button, GUID window)
		: C_InputEvent(window)
		, m_button(button) {}

	int m_button;
};

//=============================================================
class C_MouseButtonReleased : public C_MouseButtonEvent {
public:
	template<class ... Args>
	C_MouseButtonReleased(Args&&... args)
		: C_MouseButtonEvent(std::forward<Args>(args)...) {}

	EVENT_CLASS_TYPE(MouseButtonReleased);
};

//=============================================================
class C_MouseButtonPressed : public C_MouseButtonEvent {
public:
	template<class ... Args>
	C_MouseButtonPressed(Args&&... args)
		: C_MouseButtonEvent(std::forward<Args>(args)...) {}

	EVENT_CLASS_TYPE(MouseButtonPressed);
};

}}