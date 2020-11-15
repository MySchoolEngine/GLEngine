#pragma once

#include <CoreStdafx.h>

#include <Utils/BitField.h>

namespace GLEngine {
namespace Core {

//=================================================================================
enum class E_EventType {
	None = 0,
	KeyPressed, KeyReleased, KeyRepeated, TextInput,
	MouseScroll, MouseButtonReleased, MouseButtonPressed, MouseMoved,
	AppEvent, WindowClose, WindowResized,
	UserDefined,
};

//=================================================================================
enum class E_EventCategory {
	None = 0,
	Application = BIT(0),
	Input		= BIT(1),
	Keyboard	= BIT(2),
	Mouse		= BIT(3),
	MouseButton	= BIT(4),
	UserDefined = BIT(5),
};

//=================================================================================
// helpful macrso taken from TheCherno's code
#define EVENT_CLASS_TYPE(type) static E_EventType GetStaticType() { return E_EventType::##type; }\
								virtual E_EventType GetType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual Utils::C_BitField<E_EventCategory> GetCategories() const override { return category; }

//=================================================================================
class CORE_API_EXPORT I_Event
{
public:
	virtual ~I_Event();
	virtual E_EventType GetType() const = 0;
	virtual Utils::C_BitField<E_EventCategory> GetCategories() const = 0;
	virtual const char* GetName() const = 0;

	inline bool IsInCategory(E_EventCategory category) const {
		return GetCategories() & category;
	}

	bool m_Handeld = false;
};

//=================================================================================
class CORE_API_EXPORT C_UserEvent : public I_Event {
public:
	explicit C_UserEvent(const std::string& name);
	virtual ~C_UserEvent();

	virtual E_EventType GetType() const override;
	virtual Utils::C_BitField<E_EventCategory> GetCategories() const override;
	virtual const char* GetName() const override;
private:
	std::string m_Name;
};

//=================================================================================
inline std::ostream& operator<<(std::ostream& os, const I_Event& e)
{
	return os << e.GetName();
}

}

//=================================================================================
//= Event cast
//=================================================================================
template<GLEngine::Core::E_EventCategory>
class EventCategoryBase {
};

template<GLEngine::Core::E_EventCategory e,
	typename retType = typename EventCategoryBase<e>::type>
constexpr retType& event_base_cast(GLEngine::Core::I_Event& comp)
{
	return static_cast<retType&>(comp);
}

}

//=================================================================================
template<>
struct Utils::enable_BitField_operators<GLEngine::Core::E_EventCategory> {
	static constexpr bool enable = true;
};