#pragma once

#include <Core/CoreApi.h>
#include <Core/Resources/Resource.h>
#include <Core/Resources/ResourceHandle.h>
#include <Core/Resources/ResourceLoader.h>

#include <shared_mutex>
#include <type_traits>

namespace GLEngine::Core {

class CORE_API_EXPORT C_ResourceManager {
	template <class ResourceType> using T_Handle = typename std::enable_if_t<std::is_base_of_v<Resource, ResourceType>, ResourceHandle<ResourceType>>;

public:
	C_ResourceManager(C_ResourceManager const&)		= delete;
	void									operator=(C_ResourceManager const&) = delete;
	[[nodiscard]] static C_ResourceManager& Instance();

	void Destroy();

	// not needed as I run single threaded so far and does not care about memory in this moment
	void UnloadUnusedResources() {}
	void UpdatePendingLoads();

	// transfers ptr ownership to the manager
	void RegisterResourceType(const I_ResourceLoader* loader);

	template <class ResourceType> T_Handle<ResourceType> LoadResource(const std::filesystem::path filepath, bool isBlocking = false);
	template <class ResourceType> T_Handle<ResourceType> GetResource(const std::filesystem::path& filepath);

private:
	C_ResourceManager();

	void AddResourceToUnusedList(const std::shared_ptr<Resource>& resource);

	const I_ResourceLoader*	  GetLoaderForExt(const std::string& ext) const;
	std::shared_ptr<Resource> GetResourcePtr(const std::filesystem::path& filepath);

	std::map<std::filesystem::path, std::shared_ptr<Resource>> m_Resources;
	std::shared_mutex										   m_Mutex;
	std::vector<std::shared_ptr<Resource>>					   m_UnusedList;
	std::shared_mutex										   m_FinishedLoadsMutes;
	std::vector<std::shared_ptr<Resource>>					   m_FinishedLoads;
	std::vector<std::shared_ptr<Resource>>					   m_FailedLoads;

	std::map<std::string, const I_ResourceLoader*> m_ExtToLoaders;
	std::map<std::size_t, const I_ResourceLoader*> m_TypeIdToLoader;

	friend class ResourceHandleBase; // still not sure about this, I don't think I need to befriend anyone to achieve this
};

} // namespace GLEngine::Core

#include <Core/Resources/ResourceManager.inl>