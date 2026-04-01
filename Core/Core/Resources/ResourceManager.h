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
	template <class ResourceType> using T_Handle = std::enable_if_t<std::is_base_of_v<Resource, ResourceType>, ResourceHandle<ResourceType>>;

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

	template <class ResourceType> T_Handle<ResourceType> LoadResource(const std::filesystem::path& filepath, bool isBlocking = false);

	/**
	 * @brief This function will not try to load anything. Only returns handle if the resource is already loaded.
	 * @tparam ResourceType
	 * @param filepath
	 * @return
	 */
	template <class ResourceType> T_Handle<ResourceType> GetResource(const std::filesystem::path& filepath);
	std::vector<std::string>							 GetSupportedExtensions(const std::size_t) const;
	std::vector<C_Metafile>								 GetAllMetafiles(const std::filesystem::path& path = ".", bool recursive = false);

	/**
	 * @brief Returns the loader registered for the given file extension.
	 * @param ext File extension including the leading dot (e.g. ".png", ".obj").
	 * @return An engaged optional holding a reference to the loader, or std::nullopt if
	 *         @p ext is empty, does not start with '.', or has no registered loader.
	 *         The reference is valid only as long as the manager has not been destroyed.
	 */
	[[nodiscard]] std::optional<std::reference_wrapper<const I_ResourceLoader>>
		GetLoaderForExt(const std::string& ext) const;

	/**
	 * @brief Returns the loader registered for the given resource type.
	 * @tparam ResourceType Must satisfy is_resource.
	 * @return An engaged optional holding a reference to the loader, or std::nullopt if
	 *         no loader has been registered for @p ResourceType.
	 *         The reference is valid only as long as the manager has not been destroyed.
	 */
	template <class ResourceType>
		requires is_resource<ResourceType> [[nodiscard]] std::optional<std::reference_wrapper<const I_ResourceLoader>> GetLoaderForType() const;

	template <is_resource ResourceType> [[nodiscard]] bool IsResourceType(const std::filesystem::path& path) const;

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
	std::shared_ptr<Resource> GetResourcePtr(const std::filesystem::path& filepath);

	std::map<std::filesystem::path, std::shared_ptr<Resource>>		m_Resources;
	std::map<std::filesystem::path, C_Metafile>						m_Metafile; //< no access from outside of resource manager
	std::shared_mutex												m_Mutex;
	std::vector<std::pair<std::shared_ptr<Resource>, unsigned int>> m_UnusedList;
	std::shared_mutex												m_FinishedLoadsMutes;
	std::vector<std::shared_ptr<Resource>>							m_FinishedLoads;
	std::vector<std::shared_ptr<Resource>>							m_FailedLoads;

	std::map<std::string, const I_ResourceLoader*> m_ExtToLoaders;
	std::map<std::size_t, const I_ResourceLoader*> m_TypeIdToLoader;

	unsigned int						 m_UpdatesSinceLastRemove{0};
	inline static constexpr unsigned int s_NumUpdatesBetweenUnloading{10};
	inline static constexpr unsigned int s_UpdatesBeforeDelete{5};

	friend class ResourceHandleBase; // still not sure about this, I don't think I need to befriend anyone to achieve this
	friend class ResourceManagerFixture;
	friend class ResourceHandleFixture;
};

} // namespace GLEngine::Core

#include <Core/Resources/ResourceManager.inl>