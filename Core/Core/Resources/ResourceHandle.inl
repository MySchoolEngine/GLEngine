#pragma once

namespace GLEngine::Core {

//=================================================================================
template <class ResourceType> requires(is_resource<ResourceType>) ResourceType& ResourceHandle<ResourceType>::GetResource()
{
	GLE_ASSERT(IsReady(), "Trying to dereference uninitilized resource");
	return *std::static_pointer_cast<ResourceType>(m_Resource).get();
}

//=================================================================================
template <class ResourceType> requires(is_resource<ResourceType>) const ResourceType& ResourceHandle<ResourceType>::GetResource() const
{
	GLE_ASSERT(IsReady(), "Trying to dereference uninitilized resource");
	return *std::static_pointer_cast<ResourceType>(m_Resource).get();
}
} // namespace GLEngine::Core
