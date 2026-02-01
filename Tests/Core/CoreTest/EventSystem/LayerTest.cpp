#include <CoreTestStdafx.h>

#include <Core/EventSystem/Layer.h>

#include <CoreTest/EventSystem/MockLayer.h>
#include <CoreTest/EventSystem/TestEvents.h>

namespace GLEngine::Core {

//=================================================================================
// Layer Lifecycle Tests
//=================================================================================

TEST(Layer, OnAttachCanBeCalled)
{
	C_MockLayer layer;
	EXPECT_FALSE(layer.onAttachCalled);

	layer.OnAttach();

	EXPECT_TRUE(layer.onAttachCalled);
}

TEST(Layer, OnDetachCanBeCalled)
{
	C_MockLayer layer;
	EXPECT_FALSE(layer.onDetachCalled);

	layer.OnDetach();

	EXPECT_TRUE(layer.onDetachCalled);
}

TEST(Layer, OnUpdateCanBeCalledMultipleTimes)
{
	C_MockLayer layer;
	EXPECT_EQ(layer.onUpdateCallCount, 0);

	layer.OnUpdate();
	EXPECT_EQ(layer.onUpdateCallCount, 1);

	layer.OnUpdate();
	EXPECT_EQ(layer.onUpdateCallCount, 2);

	layer.OnUpdate();
	EXPECT_EQ(layer.onUpdateCallCount, 3);
}

TEST(Layer, OnEventReceivesEventReference)
{
	C_MockLayer layer;
	C_TestEvent event(42);

	layer.OnEvent(event);

	EXPECT_EQ(layer.receivedEvents.size(), 1);
	EXPECT_EQ(layer.receivedEvents[0], &event);
}

TEST(Layer, OnEventCanSetHandled)
{
	C_MockLayer layer;
	layer.shouldHandleEvent = true;

	C_TestEvent event;
	EXPECT_FALSE(event.m_Handled);

	layer.OnEvent(event);

	EXPECT_TRUE(event.m_Handled);
}

TEST(Layer, OnEventDoesNotSetHandledByDefault)
{
	C_MockLayer layer;
	// shouldHandleEvent is false by default

	C_TestEvent event;
	layer.OnEvent(event);

	EXPECT_FALSE(event.m_Handled);
}

TEST(Layer, MultipleEventsReceived)
{
	C_MockLayer layer;
	C_TestEvent event1(1);
	C_TestEvent event2(2);
	C_TestEvent event3(3);

	layer.OnEvent(event1);
	layer.OnEvent(event2);
	layer.OnEvent(event3);

	EXPECT_EQ(layer.GetReceivedEventCount(), 3);
	EXPECT_TRUE(layer.ReceivedEvent(&event1));
	EXPECT_TRUE(layer.ReceivedEvent(&event2));
	EXPECT_TRUE(layer.ReceivedEvent(&event3));
}

//=================================================================================
// ReadyForDestroy Tests
//=================================================================================

TEST(Layer, ReadyForDestroyDefaultReturnsTrue)
{
	C_MockLayer layer;
	EXPECT_TRUE(layer.ReadyForDestroy());
}

TEST(Layer, ReadyForDestroyCanReturnFalseWhenOverridden)
{
	C_MockLayer layer;
	layer.readyForDestroy = false;

	EXPECT_FALSE(layer.ReadyForDestroy());
}

TEST(Layer, ReadyForDestroyCanBeToggled)
{
	C_MockLayer layer;
	EXPECT_TRUE(layer.ReadyForDestroy());

	layer.readyForDestroy = false;
	EXPECT_FALSE(layer.ReadyForDestroy());

	layer.readyForDestroy = true;
	EXPECT_TRUE(layer.ReadyForDestroy());
}

//=================================================================================
// Layer Reset Tests
//=================================================================================

TEST(Layer, ResetClearsAllState)
{
	C_MockLayer layer;

	// Set up state
	layer.OnAttach();
	layer.OnDetach();
	layer.OnUpdate();
	layer.OnUpdate();

	C_TestEvent event;
	layer.shouldHandleEvent = true;
	layer.OnEvent(event);

	layer.readyForDestroy = false;

	// Verify state is set
	EXPECT_TRUE(layer.onAttachCalled);
	EXPECT_TRUE(layer.onDetachCalled);
	EXPECT_EQ(layer.onUpdateCallCount, 2);
	EXPECT_EQ(layer.receivedEvents.size(), 1);
	EXPECT_TRUE(layer.shouldHandleEvent);
	EXPECT_FALSE(layer.readyForDestroy);

	// Reset
	layer.Reset();

	// Verify state is cleared
	EXPECT_FALSE(layer.onAttachCalled);
	EXPECT_FALSE(layer.onDetachCalled);
	EXPECT_EQ(layer.onUpdateCallCount, 0);
	EXPECT_EQ(layer.receivedEvents.size(), 0);
	EXPECT_FALSE(layer.shouldHandleEvent);
	EXPECT_TRUE(layer.readyForDestroy);
}

//=================================================================================
// Base C_Layer Default Behavior Tests
//=================================================================================

// Test that base C_Layer has correct default behavior
// Note: We test through C_MockLayer since C_Layer::OnEvent is empty by default
TEST(LayerBase, DefaultOnEventDoesNothing)
{
	// Use C_Layer directly to test base behavior
	C_Layer		layer("BaseLayer");
	C_TestEvent event;

	// Should not crash and should not modify event
	layer.OnEvent(event);

	EXPECT_FALSE(event.m_Handled);
}

TEST(LayerBase, DefaultLifecycleMethods)
{
	C_Layer layer("BaseLayer");

	// These should not crash when called on base class
	layer.OnAttach();
	layer.OnDetach();
	layer.OnUpdate();

	// Base layer always ready for destroy
	EXPECT_TRUE(layer.ReadyForDestroy());
}

} // namespace GLEngine::Core
