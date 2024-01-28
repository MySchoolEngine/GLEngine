#pragma once

#include <Core/Resources/ResourceManager.h>

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

//=================================================================================
template <class ResourceType> requires(is_resource<ResourceType>) void ResourceHandle<ResourceType>::AfterDeserialize(GLEngine::Utils::C_XMLDeserializer::DeserializeCtx& ctx)
{
	auto& rm = C_ResourceManager::Instance();
	*this	 = rm.LoadResource<ResourceType>(GetFilePath());
}
} // namespace GLEngine::Core
