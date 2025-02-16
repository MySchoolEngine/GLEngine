#pragma once

#include <Core/EventSystem/Event.h>


// strongly inspirited by Hazel
//
namespace GLEngine::Core {

class C_EventDispatcher {
	template <typename T> using EventFn = std::function<bool(T&)>;

public:
	C_EventDispatcher(I_Event& event)
		: m_Event(event)
	{
	}
	virtual ~C_EventDispatcher() = default;


	template <typename T> bool Dispatch(EventFn<T> func)
	{
		if (!m_Event.m_Handled && m_Event.GetType() == T::GetStaticType())
		{
			m_Event.m_Handled = func(*(T*)&m_Event);
			return true;
		}
		return false;
	}

private:
	I_Event& m_Event;
};


} // namespace GLEngine::Core