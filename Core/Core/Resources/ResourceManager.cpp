#include <CoreStdafx.h>

#include <Core/Resources/ResourceManager.h>

namespace GLEngine::Core {

//=================================================================================
C_ResourceManager::C_ResourceManager() 
: C_Layer("ResourceManager")
{
}

//=================================================================================
C_ResourceManager& C_ResourceManager::Instance()
{
	static C_ResourceManager instance; // Guaranteed to be destroyed.
									   // Instantiated on first use.
	return instance;
}

//=================================================================================
void C_ResourceManager::Destroy()
{
	// TODO: cleanup maps
	// cleanup loaders
	for (const auto& [typeID, loader] : m_TypeIdToLoader)
	{
		delete loader;
	}
	m_TypeIdToLoader.clear();
	m_ExtToLoaders.clear();
}

//=================================================================================
void C_ResourceManager::RegisterResourceType(const I_ResourceLoader* loader)
{
	if (auto it = m_TypeIdToLoader.find(loader->GetResourceTypeID()); it != m_TypeIdToLoader.end())
	{
		CORE_LOG(E_Level::Error, E_Context::Core, "Loader for this type already registered");
		delete loader;
		return;
	}
	m_TypeIdToLoader[loader->GetResourceTypeID()] = loader;
	for (const auto& ext : loader->GetSupportedExtensions())
	{
		GLE_ASSERT(ext[0] == '.', "Extension needs to contain dot.");
		m_ExtToLoaders[ext] = loader;
	}
}

//=================================================================================
const I_ResourceLoader* C_ResourceManager::GetLoaderForExt(const std::string& ext) const
{
	GLE_ASSERT(ext[0] == '.', "Extension needs to contain dot.");
	if (auto loader = m_ExtToLoaders.find(ext); loader != m_ExtToLoaders.end())
	{
		return loader->second;
	}
	return nullptr;
}

//=================================================================================
std::shared_ptr<Resource> C_ResourceManager::GetResourcePtr(const std::filesystem::path& filepath)
{
	if (auto resource = m_Resources.find(filepath); resource != m_Resources.end())
	{
		return resource->second;
	}
	return nullptr;
}

//=================================================================================
void C_ResourceManager::AddResourceToUnusedList(const std::shared_ptr<Resource>& resource)
{
	std::unique_lock lock(m_Mutex);
	GLE_ASSERT(resource.use_count() == 2, "Trying to get rid of resource that is still used by others.");
	m_UnusedList.emplace_back(resource, 0);
}

//=================================================================================
void C_ResourceManager::UpdatePendingLoads()
{
	if (!m_FinishedLoadsMutes.try_lock())
	{
		return;
	}

	for (const auto& resource : m_FinishedLoads)
	{
		resource->m_State = ResourceState::Ready;
	}
	for (const auto& resource : m_FailedLoads)
	{
		resource->m_State = ResourceState::Failed;
	}

	m_FinishedLoads.clear();
	m_FailedLoads.clear();
	m_FinishedLoadsMutes.unlock();

	if (m_UpdatesSinceLastRemove > s_NumUpdatesBetweenUnloading)
	{
		UnloadUnusedResources();
		m_UpdatesSinceLastRemove = 0;
	}
	m_UpdatesSinceLastRemove++;
}

//=================================================================================
void C_ResourceManager::OnEvent(I_Event& event)
{
	for (auto& resource : m_Resources) {
		resource.second->OnEvent(event);
	}
}

//=================================================================================
void C_ResourceManager::UnloadUnusedResources()
{
	std::unique_lock lock(m_Mutex);
	// if resource is still loading, we need to finish loading
	// it is referenced from the job system, so it will be deleted after load
	for (auto& resourcePair : m_UnusedList)
	{
		// 1 reference in m_Resources, second in m_UnusedList
		if (resourcePair.second < s_UpdatesBeforeDelete)
		{
			++resourcePair.second;
			continue;
		}
		if (resourcePair.first.use_count() == 2)
		{
			// should be deleted
			const bool deleted = RemoveResource(resourcePair.first);
			GLE_ASSERT(deleted, "Unused resource not in resources");
		}
	}
	auto retainedEnd = std::remove_if(m_UnusedList.begin(), m_UnusedList.end(), [](const std::pair<std::shared_ptr<Resource>, unsigned int>& unusedResource) { 
				return unusedResource.first.use_count() < 2;
		}
	);
	m_UnusedList.erase(retainedEnd, m_UnusedList.end());
}

//=================================================================================
bool C_ResourceManager::RemoveResource(std::shared_ptr<Resource> resource)
{
	auto it = std::find_if(m_Resources.begin(), m_Resources.end(),
		[&resource](const auto& pair) { return pair.second == resource; });

	if (it != m_Resources.end())
	{
		CORE_LOG(E_Level::Info, E_Context::Core, "Removing resource {}", it->first);
		m_Resources.erase(it);
		return true;
	}
	return false;
}

//=================================================================================
C_Metafile& C_ResourceManager::GetOrCreateMetafile(const std::filesystem::path& resource)
{
	const auto metafileName = C_Metafile::GetMetafileName(resource);
	if (m_Metafile.find(metafileName) == m_Metafile.end())
	{
		// try to load and create
		C_Metafile newFile(resource);
		if (newFile.Load() == false) {
			// populate it

			// save it
			newFile.Save();
		}
		m_Metafile.emplace(metafileName, newFile);
	}
	return m_Metafile[metafileName];
}

//=================================================================================
const C_Metafile* C_ResourceManager::GetMetafile(const std::filesystem::path& resource) const
{
	const auto metafileName = C_Metafile::GetMetafileName(resource);
	if (m_Metafile.find(metafileName) == m_Metafile.end())
	{
		return nullptr;
	}
	return &m_Metafile.at(metafileName);
}

//=================================================================================
C_Metafile* C_ResourceManager::GetOrLoadMetafile(const std::filesystem::path& resource)
{
	const auto metafileName = C_Metafile::GetMetafileName(resource);
	if (m_Metafile.find(metafileName) == m_Metafile.end())
	{
		// try to load and create
		C_Metafile newFile(resource);
		if (newFile.Load())
		{
			m_Metafile.emplace(metafileName, newFile);
		}
		else
		{
			return nullptr;
		}
	}
	return &m_Metafile.at(metafileName);
}

//=================================================================================
bool I_ResourceLoader::LoadResource(const std::filesystem::path& filepath, std::shared_ptr<Resource>& resource) const
{
	return resource->Load(filepath);
}

//=================================================================================
std::vector<std::string> C_ResourceManager::GetSupportedExtensions(const std::size_t x) const
{
	auto it = m_TypeIdToLoader.find(x);
	GLE_ASSERT(m_TypeIdToLoader.contains(x), "Unregistered type");
	return it->second->GetSupportedExtensions();
}

} // namespace GLEngine::Core
