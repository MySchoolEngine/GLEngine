#pragma once

#include <CoreStdafx.h>

#include <Utils/BitField.h>

namespace GLEngine {
namespace Core {

//=================================================================================
enum class E_EventType : std::uint8_t {
	None = 0,
	KeyPressed,
	KeyReleased,
	KeyRepeated,
	TextInput,
	MouseScroll,
	MouseButtonReleased,
	MouseButtonPressed,
	MouseMoved,
	AppEvent,
	WindowClose,
	WindowResized,
	UserDefined,
	Entity,
	ResourceCreatedEvent,
	ResourceModifiedEvent,
	ResourceUnloadedEvent,
};

//=================================================================================
enum class E_EventCategory : std::uint8_t {
	None		 = 0,
	Application	 = BIT(0),
	Input		 = BIT(1),
	Keyboard	 = BIT(2),
	Mouse		 = BIT(3),
	MouseButton	 = BIT(4),
	UserDefined	 = BIT(5),
	EntitySystem = BIT(6),
	Resource	 = BIT(7),
};

//=================================================================================
// helpful macros taken from TheCherno's code
#define EVENT_CLASS_TYPE_ONLY(type)                                                                                                                                                \
	static E_EventType GetStaticType()                                                                                                                                             \
	{                                                                                                                                                                              \
		return E_EventType::type;                                                                                                                                                  \
	}                                                                                                                                                                              \
	virtual E_EventType GetType() const override                                                                                                                                   \
	{                                                                                                                                                                              \
		return GetStaticType();                                                                                                                                                    \
	}                                                                                                                                                                              \
	virtual const char* GetName() const override                                                                                                                                   \
	{                                                                                                                                                                              \
		return #type;                                                                                                                                                              \
	}

#define EVENT_CLASS_TYPE_NAME(type, name)                                                                                                                                          \
	static E_EventType GetStaticType()                                                                                                                                             \
	{                                                                                                                                                                              \
		return E_EventType::name;                                                                                                                                                  \
	}                                                                                                                                                                              \
	virtual E_EventType GetType() const override                                                                                                                                   \
	{                                                                                                                                                                              \
		return GetStaticType();                                                                                                                                                    \
	}                                                                                                                                                                              \
	virtual const char* GetName() const override                                                                                                                                   \
	{                                                                                                                                                                              \
		return name;                                                                                                                                                               \
	}

#define GET_MACRO(_1, _2, NAME, ...) NAME
#define EVENT_CLASS_TYPE(...)		 GET_MACRO(__VA_ARGS__, EVENT_CLASS_TYPE_NAME, EVENT_CLASS_TYPE_ONLY)(__VA_ARGS__)


#define EVENT_CLASS_CATEGORY(category)                                                                                                                                             \
	virtual ::Utils::C_BitField<E_EventCategory> GetCategories() const override                                                                                                    \
	{                                                                                                                                                                              \
		return category;                                                                                                                                                           \
	}

//=================================================================================
class CORE_API_EXPORT I_Event {
public:
	virtual ~I_Event();
	virtual E_EventType							 GetType() const	   = 0;
	virtual ::Utils::C_BitField<E_EventCategory> GetCategories() const = 0;
	virtual const char*							 GetName() const	   = 0;

	inline bool IsInCategory(const E_EventCategory category) const { return GetCategories() & category; }

	bool m_Handled = false;
};

//=================================================================================
class CORE_API_EXPORT C_UserEvent : public I_Event {
public:
	explicit C_UserEvent(const std::string& name);
	virtual ~C_UserEvent();

	E_EventType							 GetType() const override;
	::Utils::C_BitField<E_EventCategory> GetCategories() const override;
	const char*							 GetName() const override;

private:
	std::string m_Name;
};

//=================================================================================
class CORE_API_EXPORT C_EntityEvent : public I_Event {
public:
	enum class EntityEvent : std::uint8_t {
		Spawned,
		Despawned,

		// Editor
		Selected,

		// Entity system
		LevelLoaded,
	};

	explicit C_EntityEvent(GUID guid, EntityEvent type);

	EVENT_CLASS_TYPE(Entity)
	EVENT_CLASS_CATEGORY(E_EventCategory::EntitySystem)

private:
	GUID		m_Entity;
	EntityEvent m_Type;
};

//=================================================================================
inline std::ostream& operator<<(std::ostream& os, const I_Event& e)
{
	return os << e.GetName();
}

} // namespace Core

//=================================================================================
//= Event cast
//=================================================================================
template <Core::E_EventCategory> class EventCategoryBase {};

template <Core::E_EventCategory e, typename RetType = typename EventCategoryBase<e>::type> constexpr RetType& event_base_cast(Core::I_Event& comp)
{
	return static_cast<RetType&>(comp);
}

} // namespace GLEngine

//=================================================================================
template <> struct Utils::enable_BitField_operators<GLEngine::Core::E_EventCategory> {
	static constexpr bool enable = true;
};