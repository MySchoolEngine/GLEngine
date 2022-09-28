#include <CoreStdafx.h>

#include <Core/EventSystem/Event.h>


namespace GLEngine::Core {

//=================================================================================
// I_Event
//=================================================================================
I_Event::~I_Event() = default;

//=================================================================================
// C_UserEvent
//=================================================================================
C_UserEvent::~C_UserEvent() = default;

//=================================================================================
C_UserEvent::C_UserEvent(const std::string& name)
	: m_Name(name)
{
}

//=================================================================================
E_EventType C_UserEvent::GetType() const
{
	return Core::E_EventType::UserDefined;
}

//=================================================================================
Utils::C_BitField<GLEngine::Core::E_EventCategory> C_UserEvent::GetCategories() const
{
	return Utils::C_BitField<E_EventCategory>(E_EventCategory::UserDefined);
}

//=================================================================================
const char* C_UserEvent::GetName() const
{
	return m_Name.c_str();
}

//=================================================================================
C_EntityEvent::C_EntityEvent(GUID guid, EntityEvent type)
	: m_Entity(guid)
	, m_Type(type)
{
}

} // namespace GLEngine::Core