#include <CoreStdAfx.h>

#include <Core/Resources/ResourceManager.h>

namespace GLEngine::Core {

//=================================================================================
C_ResourceManager::C_ResourceManager()
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
	for (auto [typeID, loader] : m_TypeIdToLoader)
	{
		delete loader;
	}
	m_TypeIdToLoader.clear();
	m_ExtToLoaders.clear();
}

//=================================================================================
void C_ResourceManager::RegisterResourceType(const I_ResourceLoader* loader)
{
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
	GLE_ASSERT(resource.use_count() == 2, "Trying to get rid of resource that is still used by others.");
	m_UnusedList.push_back(resource);
}

//=================================================================================
bool I_ResourceLoader::LoadResource(const std::filesystem::path& filepath, std::shared_ptr<Resource>& resource) const
{
	return resource->Load(filepath);
}

} // namespace GLEngine::Core
