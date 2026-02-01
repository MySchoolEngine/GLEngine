#include <CoreTestStdafx.h>

#include <Core/EventSystem/LayerStack.h>

#include <CoreTest/EventSystem/MockLayer.h>
#include <CoreTest/EventSystem/TestEvents.h>

namespace GLEngine::Core {

//=================================================================================
// LayerStack Test Fixture
//=================================================================================

class LayerStackTest : public ::testing::Test {
protected:
	void SetUp() override
	{
		// Create fresh layers for each test
		layer1 = new C_MockLayer("Layer1");
		layer2 = new C_MockLayer("Layer2");
		layer3 = new C_MockLayer("Layer3");
	}

	void TearDown() override
	{
		// Note: LayerStack does not own layers (raw pointers),
		// so we must clean up manually
		delete layer1;
		delete layer2;
		delete layer3;
	}

	C_MockLayer* layer1 = nullptr;
	C_MockLayer* layer2 = nullptr;
	C_MockLayer* layer3 = nullptr;
};

//=================================================================================
// PushLayer Tests
//=================================================================================

TEST_F(LayerStackTest, AddingLayerIncludesItInUpdates)
{
	C_LayerStack stack("TestStack");
	stack.PushLayer(layer1);

	stack.OnUpdate();

	EXPECT_EQ(layer1->onUpdateCallCount, 1);
}

TEST_F(LayerStackTest, MultipleLayersAllReceiveUpdates)
{
	C_LayerStack stack("TestStack");
	stack.PushLayer(layer1);
	stack.PushLayer(layer2);
	stack.PushLayer(layer3);

	stack.OnUpdate();

	EXPECT_EQ(layer1->onUpdateCallCount, 1);
	EXPECT_EQ(layer2->onUpdateCallCount, 1);
	EXPECT_EQ(layer3->onUpdateCallCount, 1);
}

//=================================================================================
// OnUpdate Tests
//=================================================================================

TEST_F(LayerStackTest, OnUpdateCallsAllLayers)
{
	C_LayerStack stack("TestStack");
	stack.PushLayer(layer1);
	stack.PushLayer(layer2);

	stack.OnUpdate();

	EXPECT_EQ(layer1->onUpdateCallCount, 1);
	EXPECT_EQ(layer2->onUpdateCallCount, 1);
}

TEST_F(LayerStackTest, OnUpdateCallsInPushOrder)
{
	C_LayerStack stack("TestStack");

	int callOrder = 0;
	int layer1Order = 0;
	int layer2Order = 0;
	int layer3Order = 0;

	// Create custom layers that track call order
	class OrderTrackingLayer : public C_Layer {
	public:
		OrderTrackingLayer(const std::string& name, int& counter, int& myOrder)
			: C_Layer(name)
			, m_Counter(counter)
			, m_MyOrder(myOrder)
		{
		}

		void OnUpdate() override
		{
			m_MyOrder = ++m_Counter;
		}

	private:
		int& m_Counter;
		int& m_MyOrder;
	};

	OrderTrackingLayer* tracking1 = new OrderTrackingLayer("T1", callOrder, layer1Order);
	OrderTrackingLayer* tracking2 = new OrderTrackingLayer("T2", callOrder, layer2Order);
	OrderTrackingLayer* tracking3 = new OrderTrackingLayer("T3", callOrder, layer3Order);

	stack.PushLayer(tracking1);
	stack.PushLayer(tracking2);
	stack.PushLayer(tracking3);

	stack.OnUpdate();

	EXPECT_EQ(layer1Order, 1);
	EXPECT_EQ(layer2Order, 2);
	EXPECT_EQ(layer3Order, 3);

	delete tracking1;
	delete tracking2;
	delete tracking3;
}

TEST_F(LayerStackTest, OnUpdateWorksOnEmptyStack)
{
	C_LayerStack stack("TestStack");

	// Should not crash
	stack.OnUpdate();
}

TEST_F(LayerStackTest, OnUpdateCalledMultipleTimes)
{
	C_LayerStack stack("TestStack");
	stack.PushLayer(layer1);

	stack.OnUpdate();
	stack.OnUpdate();
	stack.OnUpdate();

	EXPECT_EQ(layer1->onUpdateCallCount, 3);
}

//=================================================================================
// OnEvent Propagation Tests
//=================================================================================

TEST_F(LayerStackTest, EventPropagatesToAllLayersWhenNoneHandle)
{
	C_LayerStack stack("TestStack");
	stack.PushLayer(layer1);
	stack.PushLayer(layer2);
	stack.PushLayer(layer3);

	C_TestEvent event;
	stack.OnEvent(event);

	EXPECT_TRUE(layer1->ReceivedEvent(&event));
	EXPECT_TRUE(layer2->ReceivedEvent(&event));
	EXPECT_TRUE(layer3->ReceivedEvent(&event));
}

TEST_F(LayerStackTest, EventPropagationStopsWhenHandled)
{
	C_LayerStack stack("TestStack");
	stack.PushLayer(layer1);
	stack.PushLayer(layer2);
	layer2->shouldHandleEvent = true; // Layer2 will handle the event
	stack.PushLayer(layer3);

	C_TestEvent event;
	stack.OnEvent(event);

	EXPECT_TRUE(layer1->ReceivedEvent(&event));
	EXPECT_TRUE(layer2->ReceivedEvent(&event));
	EXPECT_FALSE(layer3->ReceivedEvent(&event)); // Should not receive - stopped at layer2
	EXPECT_TRUE(event.m_Handled);
}

TEST_F(LayerStackTest, FirstLayerCanHandleAndStopPropagation)
{
	C_LayerStack stack("TestStack");
	layer1->shouldHandleEvent = true;
	stack.PushLayer(layer1);
	stack.PushLayer(layer2);
	stack.PushLayer(layer3);

	C_TestEvent event;
	stack.OnEvent(event);

	EXPECT_TRUE(layer1->ReceivedEvent(&event));
	EXPECT_FALSE(layer2->ReceivedEvent(&event));
	EXPECT_FALSE(layer3->ReceivedEvent(&event));
	EXPECT_TRUE(event.m_Handled);
}

TEST_F(LayerStackTest, MiddleLayerCanHandleAndStopPropagation)
{
	C_LayerStack stack("TestStack");
	stack.PushLayer(layer1);
	layer2->shouldHandleEvent = true;
	stack.PushLayer(layer2);
	stack.PushLayer(layer3);

	C_TestEvent event;
	stack.OnEvent(event);

	EXPECT_TRUE(layer1->ReceivedEvent(&event));
	EXPECT_TRUE(layer2->ReceivedEvent(&event));
	EXPECT_FALSE(layer3->ReceivedEvent(&event));
}

TEST_F(LayerStackTest, LastLayerCanHandle)
{
	C_LayerStack stack("TestStack");
	stack.PushLayer(layer1);
	stack.PushLayer(layer2);
	layer3->shouldHandleEvent = true;
	stack.PushLayer(layer3);

	C_TestEvent event;
	stack.OnEvent(event);

	EXPECT_TRUE(layer1->ReceivedEvent(&event));
	EXPECT_TRUE(layer2->ReceivedEvent(&event));
	EXPECT_TRUE(layer3->ReceivedEvent(&event));
	EXPECT_TRUE(event.m_Handled);
}

TEST_F(LayerStackTest, OnEventWorksOnEmptyStack)
{
	C_LayerStack stack("TestStack");
	C_TestEvent	 event;

	// Should not crash
	stack.OnEvent(event);

	EXPECT_FALSE(event.m_Handled);
}

TEST_F(LayerStackTest, PreHandledEventNotPropagated)
{
	C_LayerStack stack("TestStack");
	stack.PushLayer(layer1);
	stack.PushLayer(layer2);

	C_TestEvent event;
	event.m_Handled = true; // Pre-handled

	stack.OnEvent(event);

	// Event was already handled, so propagation stops immediately
	EXPECT_FALSE(layer1->ReceivedEvent(&event));
	EXPECT_FALSE(layer2->ReceivedEvent(&event));
}

//=================================================================================
// ReadyForDestroy Tests
//=================================================================================

TEST_F(LayerStackTest, ReadyForDestroyReturnsTrueWhenAllLayersReady)
{
	C_LayerStack stack("TestStack");
	stack.PushLayer(layer1);
	stack.PushLayer(layer2);
	stack.PushLayer(layer3);

	// All layers default to readyForDestroy = true
	EXPECT_TRUE(stack.ReadyForDestroy());
}

TEST_F(LayerStackTest, ReadyForDestroyReturnsFalseWhenAnyLayerNotReady)
{
	C_LayerStack stack("TestStack");
	stack.PushLayer(layer1);
	layer2->readyForDestroy = false;
	stack.PushLayer(layer2);
	stack.PushLayer(layer3);

	EXPECT_FALSE(stack.ReadyForDestroy());
}

TEST_F(LayerStackTest, ReadyForDestroyReturnsFalseWhenFirstLayerNotReady)
{
	C_LayerStack stack("TestStack");
	layer1->readyForDestroy = false;
	stack.PushLayer(layer1);
	stack.PushLayer(layer2);

	EXPECT_FALSE(stack.ReadyForDestroy());
}

TEST_F(LayerStackTest, ReadyForDestroyReturnsFalseWhenLastLayerNotReady)
{
	C_LayerStack stack("TestStack");
	stack.PushLayer(layer1);
	stack.PushLayer(layer2);
	layer3->readyForDestroy = false;
	stack.PushLayer(layer3);

	EXPECT_FALSE(stack.ReadyForDestroy());
}

TEST_F(LayerStackTest, ReadyForDestroyReturnsTrueOnEmptyStack)
{
	C_LayerStack stack("TestStack");
	EXPECT_TRUE(stack.ReadyForDestroy());
}

TEST_F(LayerStackTest, ReadyForDestroyRespondsToLayerStateChanges)
{
	C_LayerStack stack("TestStack");
	stack.PushLayer(layer1);

	EXPECT_TRUE(stack.ReadyForDestroy());

	layer1->readyForDestroy = false;
	EXPECT_FALSE(stack.ReadyForDestroy());

	layer1->readyForDestroy = true;
	EXPECT_TRUE(stack.ReadyForDestroy());
}

//=================================================================================
// LayerStack as Layer Tests
//=================================================================================

TEST_F(LayerStackTest, LayerStackHasName)
{
	C_LayerStack stack("MyStack");
	EXPECT_EQ(stack.GetName(), "MyStack");
}

TEST_F(LayerStackTest, LayerStackDefaultReadyForDestroy)
{
	C_LayerStack stack("TestStack");
	// Empty stack should be ready
	EXPECT_TRUE(stack.ReadyForDestroy());
}

//=================================================================================
// Multiple Events Tests
//=================================================================================

TEST_F(LayerStackTest, MultipleEventsHandledIndependently)
{
	C_LayerStack stack("TestStack");
	stack.PushLayer(layer1);
	stack.PushLayer(layer2);

	C_TestEvent event1(1);
	C_TestEvent event2(2);

	stack.OnEvent(event1);
	stack.OnEvent(event2);

	// Both layers should receive both events
	EXPECT_EQ(layer1->GetReceivedEventCount(), 2);
	EXPECT_EQ(layer2->GetReceivedEventCount(), 2);
}

TEST_F(LayerStackTest, EventHandlingStateResetsBetweenEvents)
{
	C_LayerStack stack("TestStack");
	layer1->shouldHandleEvent = true;
	stack.PushLayer(layer1);
	stack.PushLayer(layer2);

	C_TestEvent event1;
	stack.OnEvent(event1);
	EXPECT_TRUE(event1.m_Handled);
	EXPECT_FALSE(layer2->ReceivedEvent(&event1));

	// New event should start fresh
	C_TestEvent event2;
	stack.OnEvent(event2);
	EXPECT_TRUE(event2.m_Handled);
	EXPECT_FALSE(layer2->ReceivedEvent(&event2));
}

} // namespace GLEngine::Core
