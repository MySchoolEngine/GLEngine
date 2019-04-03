#pragma once

#include <CoreStdafx.h>

#include <Core/EventSystem/Event/InputEvents.h>

namespace GLEngine {
namespace Core {
//=================================================================================
class C_KeyEvent : public C_InputEvent {
public:
	inline int GetKeyCode() const { return m_KeyCode; }

protected:
	C_KeyEvent(int keyCode, GUID window)
		: C_InputEvent(window)
		, m_KeyCode(keyCode) {}
	int m_KeyCode;

	virtual Utils::C_BitField<E_EventCategory> GetInputCategory() const override
	{
		return E_EventCategory::Keyboard;
	}
};

//=================================================================================
class C_KeyPressedEvent : public C_KeyEvent {
public:
	template<class ... Args>
	C_KeyPressedEvent(Args&&... args) 
		: C_KeyEvent(std::forward<Args>(args)...) {}

	EVENT_CLASS_TYPE(KeyPressed);
};
}}