#pragma once

namespace GLEngine::Core {
//=================================================================================
template <class ResourceType> C_ResourceManager::T_Handle<ResourceType> C_ResourceManager::LoadResource(const std::filesystem::path filepath, bool isBlocking /*= false*/)
{
	// TODO: validate extension
	// check if being loaded, or already exists
	// lock maps, as we want to make this in atomic manner
	std::unique_lock lock(m_Mutex);
	if (auto resource = GetResourcePtr(filepath))
	{
		std::shared_ptr<ResourceType> concreteResource = std::static_pointer_cast<ResourceType>(resource);
		GLE_ASSERT(concreteResource, "Cannoc cast to the concrete resource type");
		return {concreteResource};
	}
	const auto* loader = GetLoaderForExt(filepath.extension().string());
	if (!loader)
	{
		CORE_LOG(E_Level::Error, E_Context::Core, "No loader specified for {}", filepath.extension());
		return {};
	}
	auto resource = loader->CreateResource();
	if (resource)
	{
		resource->m_State	  = ResourceState::Loading;
		m_Resources[filepath] = resource;
		// once all maps updated, we have to unlock, as loaders can also trigger resource loading
		// TODO: How do we propagate information isBlocking to consequent loads?
		lock.unlock();
		if (isBlocking || true)
		{
			if (loader->LoadResource(filepath, resource))
			{
				resource->m_State = ResourceState::Ready;
			}
			else
			{
				resource->m_State = ResourceState::Failed;
			}
		}
		else
		{
			// run thread, will need job manager once aviable
		}
	}

	std::shared_ptr<ResourceType> concreteResource = std::static_pointer_cast<ResourceType>(resource);
	GLE_ASSERT(concreteResource, "Cannoc cast to the concrete resource type");
	return ResourceHandle<ResourceType>(concreteResource);
}

//=================================================================================
template <class ResourceType> C_ResourceManager::T_Handle<ResourceType> C_ResourceManager::GetResource(const std::filesystem::path& filepath)
{
	if (auto resource = GetResourcePtr(filepath))
	{
		return {resource};
	}
	return {};
}

} // namespace GLEngine::Core