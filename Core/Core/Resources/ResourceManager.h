#pragma once

#include <Core/CoreApi.h>
#include <Core/EventSystem/Layer.h>
#include <Core/Resources/Metafile.h>
#include <Core/Resources/Resource.h>
#include <Core/Resources/ResourceHandle.h>
#include <Core/Resources/ResourceLoader.h>

#include <expected>
#include <functional>
#include <shared_mutex>
#include <type_traits>

namespace GLEngine::Core {

class CORE_API_EXPORT C_ResourceManager final : public C_Layer {
public:
	C_ResourceManager(const C_ResourceManager& other)	  = delete;
	C_ResourceManager(C_ResourceManager&& other) noexcept = delete;
	C_ResourceManager&						operator=(const C_ResourceManager& other) = delete;
	C_ResourceManager&						operator=(C_ResourceManager&& other) noexcept = delete;
	[[nodiscard]] static C_ResourceManager& Instance();

	void Destroy();

	// not needed as I run single threaded so far and does not care about memory at this moment
	void UnloadUnusedResources();
	void UpdatePendingLoads();

	void OnEvent(I_Event& event) override;

	// transfers ptr ownership to the manager
	void RegisterResourceType(const I_ResourceLoader* loader);

	template <IsResource ResourceType> ResourceHandle<ResourceType> LoadResource(const std::filesystem::path& filepath, bool isBlocking = false);

	/**
	 * @brief Creates new resource of type ResourceType if resource with filepath does not exist. Invalid handle otherwise.
	 *
	 * @tparam ResourceType
	 * @param filepath
	 * @return valid handle if resource with the filepath does not exist
	 */
	template <IsResource ResourceType> [[nodiscard]] ResourceHandle<ResourceType> CreateNewResource(const std::filesystem::path& filepath);
	/**
	 * @brief This function will not try to load anything. Only returns handle if the resource is already loaded.
	 * @tparam ResourceType
	 * @param filepath
	 * @return
	 */
	template <IsResource ResourceType> ResourceHandle<ResourceType> GetResource(const std::filesystem::path& filepath);
	/**
	 * @brief Due to cross DLL boundaries this function cannot be called directly with the type (from GUI) so
	 * this allows to use call of following methods through the reflection.
	 * @param hash - ResourceType::GetResourceTypeHashStatic or ResourceLoader::GetResourceTypeID
	 * @return list of supported extensions.
	 */
	std::vector<std::string> GetSupportedExtensions(const std::size_t hash) const;
	std::vector<C_Metafile>	 GetAllMetafiles(const std::filesystem::path& path = ".", bool recursive = false);

	/**
	 * @brief Returns the loader registered for the given file extension.
	 * @param ext File extension including the leading dot (e.g. ".png", ".obj").
	 * @return An engaged optional holding a reference to the loader, or std::nullopt if
	 *         @p ext is empty, does not start with '.', or has no registered loader.
	 *         The reference is valid only as long as the manager has not been destroyed.
	 */
	[[nodiscard]] std::optional<std::reference_wrapper<const I_ResourceLoader>> GetLoaderForExt(const std::string& ext) const;

	/**
	 * @brief Returns the loader registered for the given resource type.
	 * @tparam ResourceType Must satisfy is_resource.
	 * @return An engaged optional holding a reference to the loader, or std::nullopt if
	 *         no loader has been registered for @p ResourceType.
	 *         The reference is valid only as long as the manager has not been destroyed.
	 */
	template <IsResource ResourceType> [[nodiscard]] std::optional<std::reference_wrapper<const I_ResourceLoader>> GetLoaderForType() const;

	template <IsResource ResourceType> [[nodiscard]] bool IsResourceType(const std::filesystem::path& path) const;

private:
	C_ResourceManager();

	C_Metafile&		  GetOrCreateMetafile(const std::filesystem::path& resource);
	const C_Metafile* GetMetafile(const std::filesystem::path& resource) const;
	C_Metafile*		  GetOrLoadMetafile(const std::filesystem::path& resource);

	/**
	 * @brief Be careful, removes the pointer to the resource from the resource list.
	 * @param resource
	 * @return return true if success
	 */
	bool RemoveResource(std::shared_ptr<Resource> resource);

	void AddResourceToUnusedList(const std::shared_ptr<Resource>& resource);

	std::optional<std::reference_wrapper<const I_ResourceLoader>> GetLoaderForTypeID(std::size_t typeId) const;
	std::shared_ptr<Resource>									  GetResourcePtr(const std::filesystem::path& filepath);

	std::map<std::filesystem::path, std::shared_ptr<Resource>>	  m_Resources;
	std::map<std::filesystem::path, C_Metafile>					  m_Metafile; //< no access from outside of resource manager
	std::shared_mutex											  m_Mutex;
	std::list<std::pair<std::shared_ptr<Resource>, unsigned int>> m_UnusedList; // list does not invalidate the iterators on push_back
	std::shared_mutex											  m_FinishedLoadsMutes;
	std::vector<std::shared_ptr<Resource>>						  m_FinishedLoads;
	std::vector<std::shared_ptr<Resource>>						  m_FailedLoads;

	std::map<std::string, const I_ResourceLoader*> m_ExtToLoaders;
	std::map<std::size_t, const I_ResourceLoader*> m_TypeIdToLoader;

	unsigned int						 m_UpdatesSinceLastRemove{0};
	inline static constexpr unsigned int s_NumUpdatesBetweenUnloading{10};
	inline static constexpr unsigned int s_UpdatesBeforeDelete{5};

	friend class ResourceHandleBase; // still not sure about this, I don't think I need to befriend anyone to achieve this
	friend class ResourceManagerFixture;
	friend class ResourceHandleFixture;
	friend class ResourceManagerBaseFixture;
	friend class ResourcePropertyLoadingFixture; // Required for unit test access to private members
};

} // namespace GLEngine::Core

#include <Core/Resources/ResourceManager.inl>