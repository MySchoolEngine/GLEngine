#pragma once

#include <Core/EventSystem/Event.h>

namespace GLEngine::Core {

//=================================================================================
// Simple test event with UserDefined type
class C_TestEvent : public I_Event {
public:
	C_TestEvent() = default;
	explicit C_TestEvent(int value)
		: m_Value(value)
	{
	}

	int GetValue() const { return m_Value; }

	EVENT_CLASS_TYPE(UserDefined)
	EVENT_CLASS_CATEGORY(E_EventCategory::UserDefined)

private:
	int m_Value = 0;
};

//=================================================================================
// Test event with multiple categories (Input | Keyboard)
class C_MultiCategoryEvent : public I_Event {
public:
	EVENT_CLASS_TYPE(UserDefined)

	DULib::BitField<E_EventCategory> GetCategories() const override
	{
		return E_EventCategory::Input | E_EventCategory::Keyboard;
	}
};

//=================================================================================
// Alternative test event type for dispatcher tests
class C_AlternativeTestEvent : public I_Event {
public:
	EVENT_CLASS_TYPE(Entity)
	EVENT_CLASS_CATEGORY(E_EventCategory::EntitySystem)
};

} // namespace GLEngine::Core
