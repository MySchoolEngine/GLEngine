#pragma once

#include <CoreStdafx.h>

#include <Core/EventSystem/Event/InputEvents.h>
#include <Core/InputDefinitions.h>

#include <DULib/BitField.h>

namespace GLEngine::Core {
//=================================================================================
class C_KeyEvent : public C_InputEvent {
public:
	inline int GetKeyCode() const { return m_KeyCode; }

	virtual DULib::BitField<E_KeyModifiers> GetModifiers() const { return m_Modifiers; }

protected:
	C_KeyEvent(int keyCode, GUID window, DULib::BitField<E_KeyModifiers> modifiers)
		: C_InputEvent(window)
		, m_KeyCode(keyCode)
		, m_Modifiers(modifiers)
	{
	}
	int								m_KeyCode;
	DULib::BitField<E_KeyModifiers> m_Modifiers;

	DULib::BitField<E_EventCategory> GetInputCategory() const override { return E_EventCategory::Keyboard; }
};

//=================================================================================
class C_KeyPressedEvent : public C_KeyEvent {
public:
	template <class... Args>
	C_KeyPressedEvent(Args&&... args)
		: C_KeyEvent(std::forward<Args>(args)...)
	{
	}

	EVENT_CLASS_TYPE(KeyPressed)
};

//=================================================================================
class C_KeyRepeatedEvent : public C_KeyEvent {
public:
	template <class... Args>
	C_KeyRepeatedEvent(Args&&... args)
		: C_KeyEvent(std::forward<Args>(args)...)
	{
	}

	EVENT_CLASS_TYPE(KeyRepeated)
};

//=================================================================================
class C_KeyReleasedEvent : public C_KeyEvent {
public:
	template <class... Args>
	C_KeyReleasedEvent(Args&&... args)
		: C_KeyEvent(std::forward<Args>(args)...)
	{
	}

	EVENT_CLASS_TYPE(KeyReleased)
};


//=================================================================================
class C_TextInputEvent : public C_InputEvent {
public:
	C_TextInputEvent(unsigned int codepoint, GUID window)
		: C_InputEvent(window)
		, m_Codepoint(codepoint)
	{
	}

	inline int GetCodePoint() const { return m_Codepoint; }

	EVENT_CLASS_TYPE(TextInput)

protected:
	unsigned int m_Codepoint;

	DULib::BitField<E_EventCategory> GetInputCategory() const override { return E_EventCategory::Keyboard; }
};
} // namespace GLEngine::Core