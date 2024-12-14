#pragma once

#include <Core/Application.h>
#include <Core/EventSystem/Event/ResourceEvents.h>
#include <Core/Resources/Metafile.h>

namespace GLEngine::Core {
//=================================================================================
template <class ResourceType> C_ResourceManager::T_Handle<ResourceType> C_ResourceManager::LoadResource(const std::filesystem::path filepath, bool isBlocking /*= false*/)
{
	// if resource is derived, then we first need to load base resource
	// metafile will be updated with newly created resource only after the derived resource is loaded
	if constexpr (IsBeDerivedResource<ResourceType>)
	{
		if (ResourceType::IsDerived())
		{
			const C_Metafile* metafile = GetOrLoadMetafile(filepath);
			// we need to first load base resource, this can be only done with metafile
			GLE_TODO("08. 05. 2024", "RohacekD", "If we have base resource with only one extension we can load it even without the metafile");
			if (!metafile)
			{
				CORE_LOG(E_Level::Error, E_Context::Core, "No metafile for derived resource. Please, load base resource first.");
				return {};
			}
			GLE_TODO("08. 05. 2024", "RohacekD", "Propagate isBlocking, when we have job futures");
			auto baseResource = LoadResource<typename ResourceType::T_BaseResource>(metafile->GetOriginalFileName(), true);
			// this will cause problems if loaded previously as non-blocking
			if (baseResource.IsFailed())
			{
				CORE_LOG(E_Level::Error, E_Context::Core, "No metafile for derived resource. Please, load base resource first.");
				return {};
			}
		}
	}
	std::shared_ptr<ResourceType> concreteResource;
	// TODO: validate extension
	// check if being loaded, or already exists
	// lock maps, as we want to make this in atomic manner
	{
		std::unique_lock lock(m_Mutex);
		if (auto resource = GetResourcePtr(filepath))
		{
			std::shared_ptr<ResourceType> concreteResource = std::static_pointer_cast<ResourceType>(resource);
			GLE_ASSERT(concreteResource, "Cannot cast to the concrete resource type");
			return T_Handle<ResourceType>{concreteResource};
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
			if (isBlocking)
			{
				if (loader->LoadResource(filepath, resource))
				{
					resource->m_State = ResourceState::Ready;
				}
				else
				{
					if constexpr (IsBeDerivedResource<ResourceType> && BuildableResource<ResourceType>)
					{
						C_Metafile* metafile	   = GetOrLoadMetafile(filepath);
						auto			  baseResource = LoadResource<typename ResourceType::T_BaseResource>(metafile->GetOriginalFileName(), true);
						if (std::dynamic_pointer_cast<ResourceType>(resource)->Build(baseResource)) {
							resource->m_State = ResourceState::Ready;
							if (!resource->Save()) {
								CORE_LOG(E_Level::Error, E_Context::Core, "Cannot save resource {}", resource->GetFilePath());
							}
							metafile->AddDerivedResource(filepath);
							if (!metafile->Save())
							{
								CORE_LOG(E_Level::Error, E_Context::Core, "Cannot save resource metafile {}", metafile->GetMetafileName(resource->GetFilePath()));
							}

							// ResourceHandle<ResourceType> resourceHandle(resource);
							// ResourceCreatedEvent		 event(resourceHandle);
							// C_Application::Get().OnEvent(event);
						}
						else
							resource->m_State = ResourceState::Failed;
					}
					resource->m_State = ResourceState::Failed;
				}
			}
			else
			{
				// run thread, will need job manager once finished
				std::packaged_task<void(std::shared_ptr<Resource>)> load([this, filepath](std::shared_ptr<Resource> resource) {
					const auto* loader = GetLoaderForExt(filepath.extension().string());
					if (!loader)
					{
						CORE_LOG(E_Level::Error, E_Context::Core, "No loader specified for {}", filepath.extension());
						return;
					}
					const bool result = loader->LoadResource(filepath, resource);

					std::lock_guard lock(m_FinishedLoadsMutes);
					if (result)
					{
						m_FinishedLoads.push_back(resource);
						//ResourceHandle<ResourceType> resourceHandle(resource);
						//ResourceCreatedEvent event(resourceHandle);
						//C_Application::Get().OnEvent(event);
					}
					else
					{
						// put to failed
						m_FailedLoads.push_back(resource);
					}
				});

				std::thread loadThread(std::move(load), resource);
				loadThread.detach();
			}
		}

		concreteResource = std::static_pointer_cast<ResourceType>(resource);
		GLE_ASSERT(concreteResource, "Cannot cast to the concrete resource type");
	}
	const C_Metafile&			 metafile = GetOrCreateMetafile(filepath);
	ResourceHandle<ResourceType> resourceHandle(concreteResource);
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