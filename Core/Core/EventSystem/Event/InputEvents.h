#pragma once

#include <Core/EventSystem/Event.h>

namespace GLEngine {
namespace Core {

//=================================================================================
// Input event is window dependent
class C_InputEvent : public I_Event {
public:
	::Utils::C_BitField<E_EventCategory> GetCategories() const override { return GetInputCategory() | E_EventCategory::Input; }

	// window which trigger this event
	inline GUID GetWindowGUID() const { return m_WindowID; }

protected:
	C_InputEvent(GUID window)
		: m_WindowID(window)
	{
	}

	virtual ::Utils::C_BitField<E_EventCategory> GetInputCategory() const = 0;

private:
	GUID m_WindowID;
};

} // namespace Core

//=================================================================================
template <> class EventCategoryBase<Core::E_EventCategory::Input> {
public:
	using type = Core::C_InputEvent;
};
} // namespace GLEngine