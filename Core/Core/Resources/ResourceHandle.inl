#pragma once

namespace GLEngine::Core {

//=================================================================================
template <IsResource ResourceType> ResourceType& ResourceHandle<ResourceType>::GetResource()
{
	GLE_ASSERT(IsReady(), "Trying to dereference uninitialized resource");
	return *std::static_pointer_cast<ResourceType>(m_Resource).get();
}

//=================================================================================
template <IsResource ResourceType> const ResourceType& ResourceHandle<ResourceType>::GetResource() const
{
	GLE_ASSERT(IsReady(), "Trying to dereference uninitialized resource");
	return *std::static_pointer_cast<ResourceType>(m_Resource).get();
}
} // namespace GLEngine::Core
