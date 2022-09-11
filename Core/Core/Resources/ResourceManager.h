#pragma once

#include <Core/Resources/Resource.h>
#include <Core/Resources/ResourceHandle.h>

#include <shared_mutex>
#include <type_traits>

namespace GLEngine::Core {

// one resource could have multiple extensions. This means no exact mapping to resource IDs
// this forces me, to group different extensions under one loader
class CORE_API_EXPORT I_ResourceLoader {
public:
	virtual ~I_ResourceLoader()											   = default;
	virtual [[nodiscard]] std::shared_ptr<Resource> CreateResource() const = 0;
	bool											LoadResource(const std::filesystem::path& filepath, std::shared_ptr<Resource>& resource) const;
	/**
	 * List of supported extensions accompanied with the dot.
	 */
	virtual std::vector<std::string> GetSupportedExtensions() const = 0;
	virtual std::size_t				 GetResourceTypeID() const		= 0;
};

// To create loader, please inherit from this loader and register with manager
template <class ResourceType, typename = std::enable_if_t<std::is_base_of_v<Resource, ResourceType>>> class ResourceLoader : public I_ResourceLoader {
public:
	virtual std::size_t GetResourceTypeID() const final { return ResourceType::GetResourceTypeHash(); }
};

class CORE_API_EXPORT C_ResourceManager {
public:
	C_ResourceManager(C_ResourceManager const&)		= delete;
	void									operator=(C_ResourceManager const&) = delete;
	[[nodiscard]] static C_ResourceManager& Instance();

	void Destroy();

	// not needed as I run single threaded so far and does not care about memory in this moment
	void UnloadUnusedResources() {}
	void UpdatePendingLoads() {}

	// transfers ptr ownership to the manager
	void RegisterResourceType(const I_ResourceLoader* loader);

	template <class ResourceType>
	std::enable_if_t<std::is_base_of_v<Resource, ResourceType>, ResourceHandle<ResourceType>> LoadResource(const std::filesystem::path& filepath, bool isBlocking = false);
	template <class ResourceType> std::enable_if_t<std::is_base_of_v<Resource, ResourceType>, ResourceHandle<ResourceType>> GetResource(const std::filesystem::path& filepath)
	{
		if (auto resource = GetResourcePtr(filepath))
		{
			return {resource};
		}
		return {};
	}

private:
	C_ResourceManager();

	void AddResourceToUnusedList(const std::shared_ptr<Resource>& resource);

	const I_ResourceLoader*	  GetLoaderForExt(const std::string& ext) const;
	std::shared_ptr<Resource> GetResourcePtr(const std::filesystem::path& filepath);

	std::map<std::filesystem::path, std::shared_ptr<Resource>> m_Resources;
	std::shared_mutex										   m_Mutex;
	std::vector<std::shared_ptr<Resource>>					   m_UnusedList;

	std::map<std::string, const I_ResourceLoader*> m_ExtToLoaders;
	std::map<std::size_t, const I_ResourceLoader*> m_TypeIdToLoader;

	friend class ResourceHandleBase; // still not sure about this, I don't think I need to befriend anyone to achieve this
};

//=================================================================================
template <class ResourceType>
std::enable_if_t<std::is_base_of_v<Resource, ResourceType>, ResourceHandle<ResourceType>> C_ResourceManager::LoadResource(const std::filesystem::path& filepath,
																														  bool						   isBlocking /*= false*/)
{
	// TODO: validate extension
	// check if being loaded, or already exists
	// lock maps, as we want to make this in atomic manner
	std::unique_lock lock(m_Mutex);
	if (auto resource = GetResourcePtr(filepath))
	{
		return {resource};
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

	return ResourceHandle<ResourceType>(resource);
}

} // namespace GLEngine::Core
