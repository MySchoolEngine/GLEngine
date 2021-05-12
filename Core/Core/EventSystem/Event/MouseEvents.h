#pragma once

#include <CoreStdafx.h>

#include <Core/EventSystem/Event/InputEvents.h>
#include <Core/InputDefinitions.h>

namespace GLEngine::Core {

//=============================================================
class C_MouseScrollEvent : public C_InputEvent {
public:
	C_MouseScrollEvent(double dx, double dy, GUID window)
		: C_InputEvent(window)
		, dx(dx)
		, dy(dy)
	{
	}

	inline double GetXOffset() const { return dx; }
	inline double GetYOffset() const { return dy; }

	virtual Utils::C_BitField<E_EventCategory> GetInputCategory() const override { return E_EventCategory::Mouse; }
	EVENT_CLASS_TYPE(MouseScroll);

private:
	double dx, dy;
};

//=============================================================
class C_MouseMoved : public C_InputEvent {
public:
	C_MouseMoved(float posX, float posY, GUID window)
		: C_InputEvent(window)
		, m_PosX(posX)
		, m_PosY(posY)
	{
	}

	[[nodiscard]] inline float	   GetPosX() const { return m_PosX; }
	[[nodiscard]] inline float	   GetPosY() const { return m_PosY; }
	[[nodiscard]] inline glm::vec2 GetPosition() const { return {GetPosX(), GetPosY()}; }

	virtual Utils::C_BitField<E_EventCategory> GetInputCategory() const override { return E_EventCategory::Mouse; }

	EVENT_CLASS_TYPE(MouseMoved);

private:
	float m_PosX;
	float m_PosY;
};

//=============================================================
class C_MouseButtonEvent : public C_InputEvent {
public:
	int GetMouseButton() const { return m_button; }

	virtual Utils::C_BitField<E_EventCategory> GetInputCategory() const override { return E_EventCategory::Mouse | E_EventCategory::MouseButton; }

	virtual Utils::C_BitField<E_KeyModifiers> GetModifiers() const { return m_Modifiers; }

protected:
	C_MouseButtonEvent(int button, GUID window, Utils::C_BitField<E_KeyModifiers> modifiers)
		: C_InputEvent(window)
		, m_button(button)
		, m_Modifiers(modifiers)
	{
	}

	int								  m_button;
	Utils::C_BitField<E_KeyModifiers> m_Modifiers;
};

//=============================================================
class C_MouseButtonReleased : public C_MouseButtonEvent {
public:
	template <class... Args>
	C_MouseButtonReleased(Args&&... args)
		: C_MouseButtonEvent(std::forward<Args>(args)...)
	{
	}

	EVENT_CLASS_TYPE(MouseButtonReleased);
};

//=============================================================
class C_MouseButtonPressed : public C_MouseButtonEvent {
public:
	template <class... Args>
	C_MouseButtonPressed(Args&&... args)
		: C_MouseButtonEvent(std::forward<Args>(args)...)
	{
	}

	EVENT_CLASS_TYPE(MouseButtonPressed);
};

} // namespace GLEngine::Core