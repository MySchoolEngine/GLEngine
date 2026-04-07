#pragma once

#include <Core/EventSystem/Event/ResourceEvents.h>
#include <Core/Resources/Metafile.h>

namespace GLEngine::Core {
//=================================================================================
template <class ResourceType> C_ResourceManager::T_Handle<ResourceType> C_ResourceManager::LoadResource(const std::filesystem::path& filepath, bool isBlocking /*= false*/)
{
	const auto filepathNormalized = std::filesystem::relative(filepath, "./");
	// if resource is derived, then we first need to load base resource
	// metafile will be updated with newly created resource only after the derived resource is loaded
	if constexpr (IsBeDerivedResource<ResourceType>)
	{
		if (ResourceType::IsDerived())
		{
			const C_Metafile* metafile = GetOrLoadMetafile(filepathNormalized);
			// we need to first load base resource, this can be only done with metafile
			GLE_TODO("08. 05. 2024", "RohacekD", "If we have base resource with only one extension we can load it even without the metafile");
			if (!metafile)
			{
				CORE_LOG(E_Level::Error, E_Context::Core, "No metafile for derived resource. Please, load base resource first.");
				return {};
			}
			GLE_TODO("08. 05. 2024", "RohacekD", "Propagate isBlocking, when we have job futures");
			const auto baseResource = LoadResource<typename ResourceType::T_BaseResource>(metafile->GetOriginalFileName(), true);
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
		if (const auto resource = GetResourcePtr(filepathNormalized))
		{
			concreteResource = std::dynamic_pointer_cast<ResourceType>(resource);
			if (!concreteResource)
			{
				CORE_LOG(E_Level::Error, E_Context::Core, "Resource {} is of different type than requested.", filepathNormalized);
				return {};
			}
			return T_Handle<ResourceType>{concreteResource};
		}
		const auto loaderOpt = GetLoaderForExt(filepathNormalized.extension().string());
		if (!loaderOpt)
		{
			CORE_LOG(E_Level::Error, E_Context::Core, "No loader specified for {}", filepathNormalized.extension());
			return {};
		}
		const auto loader	= loaderOpt.value();
		auto resource = loader.get().CreateResource();
		if (resource)
		{
			resource->m_State	  = ResourceState::Loading;
			m_Resources[filepathNormalized] = resource;
			// once all maps updated, we have to unlock, as loaders can also trigger resource loading
			// TODO: How do we propagate information isBlocking to consequent loads?
			lock.unlock();
			if (isBlocking)
			{
				if (loader.get().LoadResource(filepathNormalized, resource))
				{
					resource->m_State = ResourceState::Ready;
				}
				else
				{
					if constexpr (IsBeDerivedResource<ResourceType> && BuildableResource<ResourceType>)
					{
						C_Metafile* metafile		   = GetOrLoadMetafile(filepathNormalized);
						const auto	baseResourceHandle = LoadResource<typename ResourceType::T_BaseResource>(metafile->GetOriginalFileName(), true);
						if (baseResourceHandle.IsReady() == false)
						{
							CORE_LOG(E_Level::Error, E_Context::Core, "File {} doesn't load properly, thus cannot build derived resource {}", metafile->GetOriginalFileName(),
									 filepathNormalized);
							RemoveResource(resource);
							return {};
						}
						if (std::dynamic_pointer_cast<ResourceType>(resource)->Build(baseResourceHandle.GetResource()))
						{
							resource->m_State = ResourceState::Ready;
							if (!resource->Save())
							{
								CORE_LOG(E_Level::Error, E_Context::Core, "Cannot save resource {}", resource->GetFilePath());
							}
							metafile->AddDerivedResource(filepathNormalized);
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
					else
					{
						resource->m_State = ResourceState::Failed;
					}
				}
			}
			else
			{
				// run thread, will need job manager once finished
				std::packaged_task<void(std::shared_ptr<Resource>)> load([this, filepathNormalized](std::shared_ptr<Resource> resource) {
					const auto loader = GetLoaderForExt(filepathNormalized.extension().string());
					if (!loader)
					{
						CORE_LOG(E_Level::Error, E_Context::Core, "No loader specified for {}", filepathNormalized.extension());
						return;
					}
					const bool result = loader->get().LoadResource(filepathNormalized, resource);

					std::lock_guard lock(m_FinishedLoadsMutes);
					if (result)
					{
						m_FinishedLoads.push_back(resource);
						// ResourceHandle<ResourceType> resourceHandle(resource);
						// ResourceCreatedEvent event(resourceHandle);
						// C_Application::Get().OnEvent(event);
					}
					else
					{
						if constexpr (IsBeDerivedResource<ResourceType> && BuildableResource<ResourceType>)
						{
							C_Metafile* metafile		   = GetOrLoadMetafile(filepathNormalized);
							const auto	baseResourceHandle = GetResource<typename ResourceType::T_BaseResource>(metafile->GetOriginalFileName());
							if (baseResourceHandle.IsReady() == false)
							{
								CORE_LOG(E_Level::Error, E_Context::Core,
										 "File {} is not already loaded, thus cannot build derived resource {}. Buildable resources are only supported in non-async calls.",
										 metafile->GetOriginalFileName(), filepathNormalized);
								RemoveResource(resource);
								// put to failed
								m_FailedLoads.push_back(resource);
								return;
							}
							if (std::dynamic_pointer_cast<ResourceType>(resource)->Build(baseResourceHandle.GetResource()))
							{
								if (!resource->Save())
								{
									CORE_LOG(E_Level::Error, E_Context::Core, "Cannot save resource {}", resource->GetFilePath());
								}
								metafile->AddDerivedResource(filepathNormalized);
								m_FinishedLoads.push_back(resource);
								return;
							}

							CORE_LOG(E_Level::Error, E_Context::Core, "Buildable resource is not possible to load async {}", filepathNormalized);
						}
						// put to failed
						m_FailedLoads.push_back(resource);
					}
				});

				std::thread loadThread(std::move(load), resource);
				loadThread.detach();
			}
		}

		concreteResource = std::dynamic_pointer_cast<ResourceType>(resource);
		if (!concreteResource)
		{
			CORE_LOG(E_Level::Error, E_Context::Core, "Resource {} is of different type than requested.", filepathNormalized);
			RemoveResource(resource); // TODO shouldn't been added?
			return {};
		}
	}
	GetOrCreateMetafile(filepathNormalized);
	return ResourceHandle<ResourceType>(concreteResource);
}

//=================================================================================
template <class ResourceType> C_ResourceManager::T_Handle<ResourceType> C_ResourceManager::GetResource(const std::filesystem::path& filepath)
{
	const auto filepathNormalized = std::filesystem::relative(filepath, "./");
	if (auto resource = GetResourcePtr(filepathNormalized))
	{
		auto concreteResource = std::dynamic_pointer_cast<ResourceType>(resource);
		GLE_ASSERT(concreteResource, "Cannot cast to the concrete resource type");
		if (!concreteResource)
			return {};
		return T_Handle<ResourceType>{concreteResource};
	}
	return T_Handle<ResourceType>{};
}

//=================================================================================
template <class ResourceType>
requires is_resource<ResourceType> [[nodiscard]] std::optional<std::reference_wrapper<const I_ResourceLoader>> C_ResourceManager::GetLoaderForType() const
{
	// use rttr to avoid linking all libraries together
	const auto resourceType = rttr::type::get<ResourceType>();
	const auto x			= resourceType.get_method("GetResourceTypeHashStatic").invoke({}).template get_value<std::size_t>();
	return GetLoaderForTypeID(x);
}

//=================================================================================
template <is_resource ResourceType> [[nodiscard]] bool C_ResourceManager::IsResourceType(const std::filesystem::path& path) const
{
	const auto loader = GetLoaderForExt(path.extension().generic_string());
	if (loader.has_value() == false)
		return false;

	// currently I only support image resources
	return ResourceType::GetResourceTypeHashStatic() == loader->get().GetResourceTypeID();
}

} // namespace GLEngine::Core