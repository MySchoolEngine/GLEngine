#include <CoreTestStdafx.h>

#include <Core/EventSystem/Event.h>
#include <Core/EventSystem/EventDispatcher.h>

#include <CoreTest/EventSystem/TestEvents.h>

namespace GLEngine::Core {

//=================================================================================
// EventDispatcher Tests
//=================================================================================

class EventDispatcherTest : public ::testing::Test {
public:

	bool TestEventHandler(C_TestEvent& event)
	{
		handlerCalled		 = true;
		handlerReceivedValue = event.GetValue();
		return true;
	}

protected:
	bool handlerCalled		  = false;
	int	 handlerReceivedValue = 0;

	bool NonHandlingHandler(C_TestEvent& event)
	{
		handlerCalled		  = true;
		handlerReceivedValue = event.GetValue();
		return false; // Does not handle the event
	}

	bool AlternativeEventHandler(C_AlternativeTestEvent& event)
	{
		handlerCalled = true;
		return true;
	}
};

TEST_F(EventDispatcherTest, DispatchWithMatchingTypeReturnsTrue)
{
	C_TestEvent		   event(42);
	C_EventDispatcher dispatcher(event);

	bool dispatched = dispatcher.Dispatch<C_TestEvent>(
		[this](C_TestEvent& e) { return TestEventHandler(e); });

	EXPECT_TRUE(dispatched);
}

TEST_F(EventDispatcherTest, DispatchWithMatchingTypeCallsHandler)
{
	C_TestEvent		   event(42);
	C_EventDispatcher dispatcher(event);

	dispatcher.Dispatch<C_TestEvent>(
		[this](C_TestEvent& e) { return TestEventHandler(e); });

	EXPECT_TRUE(handlerCalled);
}

TEST_F(EventDispatcherTest, DispatchWithNonMatchingTypeReturnsFalse)
{
	C_TestEvent		   event(42);
	C_EventDispatcher dispatcher(event);

	bool dispatched = dispatcher.Dispatch<C_AlternativeTestEvent>(
		[this](C_AlternativeTestEvent& e) { return AlternativeEventHandler(e); });

	EXPECT_FALSE(dispatched);
}

TEST_F(EventDispatcherTest, DispatchWithNonMatchingTypeDoesNotCallHandler)
{
	C_TestEvent		   event(42);
	C_EventDispatcher dispatcher(event);

	dispatcher.Dispatch<C_AlternativeTestEvent>(
		[this](C_AlternativeTestEvent& e) { return AlternativeEventHandler(e); });

	EXPECT_FALSE(handlerCalled);
}

TEST_F(EventDispatcherTest, SetsHandledWhenHandlerReturnsTrue)
{
	C_TestEvent		   event(42);
	C_EventDispatcher dispatcher(event);

	dispatcher.Dispatch<C_TestEvent>(
		[this](C_TestEvent& e) { return TestEventHandler(e); });

	EXPECT_TRUE(event.m_Handled);
}

TEST_F(EventDispatcherTest, DoesNotSetHandledWhenHandlerReturnsFalse)
{
	C_TestEvent		   event(42);
	C_EventDispatcher dispatcher(event);

	dispatcher.Dispatch<C_TestEvent>(
		[this](C_TestEvent& e) { return NonHandlingHandler(e); });

	EXPECT_FALSE(event.m_Handled);
}

TEST_F(EventDispatcherTest, SkipsAlreadyHandledEvents)
{
	C_TestEvent event(42);
	event.m_Handled = true; // Pre-mark as handled

	C_EventDispatcher dispatcher(event);

	bool dispatched = dispatcher.Dispatch<C_TestEvent>(
		[this](C_TestEvent& e) { return TestEventHandler(e); });

	EXPECT_FALSE(dispatched);
	EXPECT_FALSE(handlerCalled);
}

TEST_F(EventDispatcherTest, PassesCorrectEventReferenceToHandler)
{
	const int		   expectedValue = 123;
	C_TestEvent		   event(expectedValue);
	C_EventDispatcher dispatcher(event);

	dispatcher.Dispatch<C_TestEvent>(
		[this](C_TestEvent& e) { return TestEventHandler(e); });

	EXPECT_EQ(handlerReceivedValue, expectedValue);
}

TEST_F(EventDispatcherTest, MultipleDispatchCallsOnDifferentTypes)
{
	C_TestEvent		   event(42);
	C_EventDispatcher dispatcher(event);

	bool testHandlerCalled		  = false;
	bool alternativeHandlerCalled = false;

	// First dispatch - should match
	dispatcher.Dispatch<C_TestEvent>(
		[&testHandlerCalled](C_TestEvent& e)
		{
			testHandlerCalled = true;
			return true;
		});

	// Second dispatch - should not match (event is C_TestEvent, not C_AlternativeTestEvent)
	dispatcher.Dispatch<C_AlternativeTestEvent>(
		[&alternativeHandlerCalled](C_AlternativeTestEvent& e)
		{
			alternativeHandlerCalled = true;
			return true;
		});

	EXPECT_TRUE(testHandlerCalled);
	EXPECT_FALSE(alternativeHandlerCalled);
}

TEST_F(EventDispatcherTest, DispatchStopsAfterEventHandled)
{
	C_TestEvent		   event(42);
	C_EventDispatcher dispatcher(event);

	int handlerCallCount = 0;

	// First dispatch - handles the event
	dispatcher.Dispatch<C_TestEvent>(
		[&handlerCallCount](C_TestEvent& e)
		{
			++handlerCallCount;
			return true;
		});

	// Second dispatch of same type - should be skipped because event is handled
	dispatcher.Dispatch<C_TestEvent>(
		[&handlerCallCount](C_TestEvent& e)
		{
			++handlerCallCount;
			return true;
		});

	EXPECT_EQ(handlerCallCount, 1);
	EXPECT_TRUE(event.m_Handled);
}

TEST_F(EventDispatcherTest, DispatchWithLambda)
{
	C_TestEvent		   event(99);
	C_EventDispatcher dispatcher(event);

	int capturedValue = 0;
	dispatcher.Dispatch<C_TestEvent>(
		[&capturedValue](C_TestEvent& e)
		{
			capturedValue = e.GetValue();
			return true;
		});

	EXPECT_EQ(capturedValue, 99);
}

TEST_F(EventDispatcherTest, DispatchWithStdBind)
{
	C_TestEvent		   event(77);
	C_EventDispatcher dispatcher(event);

	dispatcher.Dispatch<C_TestEvent>(
		std::bind(&EventDispatcherTest::TestEventHandler, this, std::placeholders::_1));

	EXPECT_TRUE(handlerCalled);
	EXPECT_EQ(handlerReceivedValue, 77);
}

} // namespace GLEngine::Core
