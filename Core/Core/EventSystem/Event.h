#pragma once

#include <CoreStdafx.h>

#include <Utils/BitField.h>

namespace GLEngine {
namespace Core {

//=================================================================================
enum class E_EventType {
	None = 0,
	KeyPressed, KeyReleased, KeyRepeated,
	MouseScroll, MouseButtonReleased, MouseButtonPressed, MouseMoved,
	WindowClose,
};

//=================================================================================
enum class E_EventCategory {
	None = 0,
	Application = BIT(0),
	Input		= BIT(1),
	Keyboard	= BIT(2),
	Mouse		= BIT(3),
	MouseButton	= BIT(4),
};

//=================================================================================
// helpful macrso taken from TheCherno's code
#define EVENT_CLASS_TYPE(type) static E_EventType GetStaticType() { return E_EventType::##type; }\
								virtual E_EventType GetType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

//=================================================================================
class API_EXPORT I_Event
{
public:
	virtual E_EventType GetType() const = 0;
	virtual Utils::C_BitField<E_EventCategory> GetCategories() const = 0;
	virtual const char* GetName() const = 0;

	inline bool IsInCategory(E_EventCategory category) const {
		return GetCategories() & category;
	}

	bool m_Handeld = false;
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
	typename retType = EventCategoryBase<e>::type>
	constexpr retType& event_base_cast(GLEngine::Core::I_Event& comp) {
	return static_cast<typename retType&>(comp);
}

}

//=================================================================================
template<>
struct Utils::enable_BitField_operators<GLEngine::Core::E_EventCategory> {
	static constexpr bool enable = true;
};