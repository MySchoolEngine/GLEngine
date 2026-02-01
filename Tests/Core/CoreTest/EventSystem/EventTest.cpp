#include <CoreTestStdafx.h>

#include <Core/EventSystem/Event.h>
#include <Core/EventSystem/Event/AppEvent.h>
#include <Core/EventSystem/Event/InputEvents.h>
#include <Core/EventSystem/Event/KeyboardEvents.h>
#include <Core/EventSystem/Event/MouseEvents.h>

#include <CoreTest/EventSystem/TestEvents.h>

#include <sstream>

namespace GLEngine::Core {

//=================================================================================
// I_Event Interface Tests
//=================================================================================

TEST(IEvent, HandledDefaultsToFalse)
{
	C_TestEvent event;
	EXPECT_FALSE(event.m_Handled);
}

TEST(IEvent, IsInCategoryReturnsTrueForMatchingCategory)
{
	C_TestEvent event;
	EXPECT_TRUE(event.IsInCategory(E_EventCategory::UserDefined));
}

TEST(IEvent, IsInCategoryReturnsFalseForNonMatchingCategory)
{
	C_TestEvent event;
	EXPECT_FALSE(event.IsInCategory(E_EventCategory::Input));
	EXPECT_FALSE(event.IsInCategory(E_EventCategory::Keyboard));
	EXPECT_FALSE(event.IsInCategory(E_EventCategory::Mouse));
}

TEST(IEvent, MultipleCategoriesMatchAll)
{
	C_MultiCategoryEvent event;
	EXPECT_TRUE(event.IsInCategory(E_EventCategory::Input));
	EXPECT_TRUE(event.IsInCategory(E_EventCategory::Keyboard));
	EXPECT_FALSE(event.IsInCategory(E_EventCategory::Mouse));
}

//=================================================================================
// C_UserEvent Tests
//=================================================================================

TEST(UserEvent, GetType)
{
	C_UserEvent event("Test");
	EXPECT_EQ(event.GetType(), E_EventType::UserDefined);
}

TEST(UserEvent, GetCategories)
{
	C_UserEvent event("Test");
	EXPECT_TRUE(event.IsInCategory(E_EventCategory::UserDefined));
}

//=================================================================================
// C_EntityEvent Tests
//=================================================================================

TEST(EntityEvent, GetStaticType)
{
	EXPECT_EQ(C_EntityEvent::GetStaticType(), E_EventType::Entity);
}

TEST(EntityEvent, GetType)
{
	C_EntityEvent event(NextGUID(), C_EntityEvent::EntityEvent::Selected);
	EXPECT_EQ(event.GetType(), E_EventType::Entity);
}

TEST(EntityEvent, GetCategories)
{
	C_EntityEvent event(NextGUID(), C_EntityEvent::EntityEvent::LevelLoaded);
	EXPECT_TRUE(event.IsInCategory(E_EventCategory::EntitySystem));
}

//=================================================================================
// C_KeyPressedEvent Tests
//=================================================================================

TEST(KeyPressedEvent, Categories)
{
	C_KeyPressedEvent event(65, NextGUID(), E_KeyModifiers::Shift);

	EXPECT_TRUE(event.IsInCategory(E_EventCategory::Input));
	EXPECT_TRUE(event.IsInCategory(E_EventCategory::Keyboard));
	EXPECT_FALSE(event.IsInCategory(E_EventCategory::Mouse));
}

TEST(KeyPressedEvent, StaticType)
{
	EXPECT_EQ(C_KeyPressedEvent::GetStaticType(), E_EventType::KeyPressed);
}

//=================================================================================
// C_KeyReleasedEvent Tests
//=================================================================================

TEST(KeyReleasedEvent, TypeIdentification)
{
	C_KeyReleasedEvent event(65, NextGUID(), E_KeyModifiers::Shift);
	EXPECT_EQ(event.GetType(), E_EventType::KeyReleased);
	EXPECT_EQ(C_KeyReleasedEvent::GetStaticType(), E_EventType::KeyReleased);
}

//=================================================================================
// C_KeyRepeatedEvent Tests
//=================================================================================

TEST(KeyRepeatedEvent, TypeIdentification)
{
	C_KeyRepeatedEvent event(65, NextGUID(), E_KeyModifiers::Shift);
	EXPECT_EQ(event.GetType(), E_EventType::KeyRepeated);
	EXPECT_EQ(C_KeyRepeatedEvent::GetStaticType(), E_EventType::KeyRepeated);
}

//=================================================================================
// C_TextInputEvent Tests
//=================================================================================

TEST(TextInputEvent, Categories)
{
	C_TextInputEvent event(65, NextGUID());
	EXPECT_TRUE(event.IsInCategory(E_EventCategory::Input));
	EXPECT_TRUE(event.IsInCategory(E_EventCategory::Keyboard));
}

//=================================================================================
// C_MouseScrollEvent Tests
//=================================================================================

TEST(MouseScrollEvent, Categories)
{
	C_MouseScrollEvent event(0.0, 0.0, NextGUID());
	EXPECT_TRUE(event.IsInCategory(E_EventCategory::Input));
	EXPECT_TRUE(event.IsInCategory(E_EventCategory::Mouse));
	EXPECT_FALSE(event.IsInCategory(E_EventCategory::MouseButton));
}

//=================================================================================
// C_MouseMoved Tests
//=================================================================================

TEST(MouseMoved, Categories)
{
	C_MouseMoved event(0.0f, 0.0f, NextGUID());
	EXPECT_TRUE(event.IsInCategory(E_EventCategory::Input));
	EXPECT_TRUE(event.IsInCategory(E_EventCategory::Mouse));
	EXPECT_FALSE(event.IsInCategory(E_EventCategory::MouseButton));
}

//=================================================================================
// C_MouseButtonPressed Tests
//=================================================================================

TEST(MouseButtonPressed, Categories)
{
	C_MouseButtonPressed event(0, NextGUID(), E_KeyModifiers::Shift);
	EXPECT_TRUE(event.IsInCategory(E_EventCategory::Input));
	EXPECT_TRUE(event.IsInCategory(E_EventCategory::Mouse));
	EXPECT_TRUE(event.IsInCategory(E_EventCategory::MouseButton));
}

//=================================================================================
// C_MouseButtonReleased Tests
//=================================================================================

TEST(MouseButtonReleased, Categories)
{
	C_MouseButtonReleased event(0, NextGUID(), E_KeyModifiers::Shift);
	EXPECT_TRUE(event.IsInCategory(E_EventCategory::Input));
	EXPECT_TRUE(event.IsInCategory(E_EventCategory::Mouse));
	EXPECT_TRUE(event.IsInCategory(E_EventCategory::MouseButton));
}

//=================================================================================
// C_AppEvent Tests
//=================================================================================

TEST(AppEvent, SetEventType)
{
	C_AppEvent event(C_AppEvent::E_Type::AppInit);
	event.SetEventType(C_AppEvent::E_Type::WindowCloseRequest);
	EXPECT_EQ(event.GetEventType(), C_AppEvent::E_Type::WindowCloseRequest);
}

TEST(AppEvent, StaticType)
{
	EXPECT_EQ(C_AppEvent::GetStaticType(), E_EventType::AppEvent);
}

//=================================================================================
// C_WindowResizedEvent Tests
//=================================================================================

TEST(WindowResizedEvent, Categories)
{
	C_WindowResizedEvent event(800, 600);
	EXPECT_TRUE(event.IsInCategory(E_EventCategory::Application));
}

TEST(WindowResizedEvent, StaticType)
{
	EXPECT_EQ(C_WindowResizedEvent::GetStaticType(), E_EventType::WindowResized);
}

//=================================================================================
// Stream Operator Tests
//=================================================================================

TEST(EventStreamOperator, OutputsEventName)
{
	C_TestEvent		  event;
	std::stringstream ss;
	ss << event;
	EXPECT_EQ(ss.str(), "UserDefined");
}

TEST(EventStreamOperator, OutputsEntityEventName)
{
	C_EntityEvent	  event(NextGUID(), C_EntityEvent::EntityEvent::Spawned);
	std::stringstream ss;
	ss << event;
	EXPECT_EQ(ss.str(), "Entity");
}

} // namespace GLEngine::Core
