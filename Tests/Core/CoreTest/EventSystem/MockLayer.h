#pragma once

#include <Core/EventSystem/Layer.h>

#include <vector>

namespace GLEngine::Core {

//=================================================================================
// Mock layer implementation for testing lifecycle methods and event handling
class C_MockLayer : public C_Layer {
public:
	explicit C_MockLayer(const std::string& name = "MockLayer")
		: C_Layer(name)
	{
	}

	// Lifecycle tracking
	bool onAttachCalled  = false;
	bool onDetachCalled  = false;
	int	 onUpdateCallCount = 0;

	// Event tracking
	std::vector<I_Event*> receivedEvents;
	bool				  shouldHandleEvent = false;

	// ReadyForDestroy control
	bool readyForDestroy = true;

	//===========================================
	// C_Layer overrides
	//===========================================
	void OnAttach() override
	{
		onAttachCalled = true;
	}

	void OnDetach() override
	{
		onDetachCalled = true;
	}

	void OnUpdate() override
	{
		++onUpdateCallCount;
	}

	void OnEvent(Core::I_Event& event) override
	{
		receivedEvents.push_back(&event);
		if (shouldHandleEvent)
		{
			event.m_Handled = true;
		}
	}

	[[nodiscard]] bool ReadyForDestroy() const override
	{
		return readyForDestroy;
	}

	//===========================================
	// Test helpers
	//===========================================
	void Reset()
	{
		onAttachCalled	   = false;
		onDetachCalled	   = false;
		onUpdateCallCount  = 0;
		receivedEvents.clear();
		shouldHandleEvent  = false;
		readyForDestroy	   = true;
	}

	bool ReceivedEvent(I_Event* event) const
	{
		return std::find(receivedEvents.begin(), receivedEvents.end(), event) != receivedEvents.end();
	}

	size_t GetReceivedEventCount() const
	{
		return receivedEvents.size();
	}
};

} // namespace GLEngine::Core
