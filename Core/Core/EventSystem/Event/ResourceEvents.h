#pragma once

#include <CoreStdafx.h>

#include <Core/EventSystem/Event.h>
#include <Core/Resources/ResourceHandle.h>

namespace GLEngine {
namespace Core {

//=================================================================================
class BaseResourceEvent : public I_Event {
public:
	BaseResourceEvent(ResourceHandleBase handle)
		: m_Handle(handle)
	{}
	~BaseResourceEvent() = default;


	::Utils::C_BitField<E_EventCategory> GetCategories() const override { return {E_EventCategory::Resource}; }

protected:
	ResourceHandleBase m_Handle;
};

template <is_resource ResourceType>
class ResourceEvent : public BaseResourceEvent {
public:
	ResourceEvent(ResourceHandle<ResourceType> handle)
		: BaseResourceEvent(handle)
	{
	}
	~ResourceEvent() = default;

	ResourceHandle<ResourceType> GetResourceHandle() const { return dynamic_cast<ResourceHandle<ResourceType>>(m_Handle); }
};

template <class ResourceType> requires(is_resource<ResourceType>)
class ResourceCreatedEvent : public ResourceEvent<ResourceType> {
public:
	ResourceCreatedEvent(ResourceHandle<ResourceType> handle)
		: ResourceEvent<ResourceType>(handle)
	{
	}
	~ResourceCreatedEvent() = default;

	EVENT_CLASS_TYPE(ResourceCreatedEvent);
};

template <class ResourceType> requires(is_resource<ResourceType>)
class ResourceUnloadedEvent : public ResourceEvent<ResourceType> {
public:
	ResourceUnloadedEvent(ResourceHandle<ResourceType> handle)
		: ResourceEvent<ResourceType>(handle)
	{
	}
	~ResourceUnloadedEvent() = default;

	EVENT_CLASS_TYPE(ResourceUnloadedEvent);
};

template <class ResourceType> requires(is_resource<ResourceType>)
class ResourceModifiedEvent : public ResourceEvent<ResourceType> {
public:
	ResourceModifiedEvent(ResourceHandle<ResourceType> handle)
		: ResourceEvent<ResourceType>(handle)
	{
	}
	~ResourceModifiedEvent() = default;

	EVENT_CLASS_TYPE(ResourceModifiedEvent);
};

} // namespace Core

//=================================================================================
template <> class EventCategoryBase<Core::E_EventCategory::Resource> {
public:
	using type = Core::BaseResourceEvent;
};
} // namespace GLEngine