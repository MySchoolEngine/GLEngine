#pragma once

#include <Core/CoreApi.h>
#include <Core/EventSystem/EventReciever.h>

#include <rttr/registration_friend.h>

#include <cstdint>
#include <concepts>
#include <memory>
#include <filesystem>


/**
 * Allows usage of resource handle inside the automatic deserialization. Actively loads resource after class deserialization.
 * 
 * @param resourceType 
 */
#define DEFINE_RESOURCE_HANDLE_METHODS(resourceType)                                                                                                                               \
	namespace GLEngine::Core {                                                                                                                                                     \
	template <> void ResourceHandle<resourceType>::AfterDeserialize(Utils::C_XMLDeserializer::DeserializeCtx& ctx)                                                                 \
	{                                                                                                                                                                              \
		*this = ctx.m_ResMng.LoadResource<resourceType>(GetFilePath());                                                                                                            \
	}                                                                                                                                                                              \
	} // namespace GLEngine::Core

#define DECLARE_RESOURCE_CONVERTOR(resourceType)                                                                                                                                   \
	rttr::type::register_wrapper_converter_for_base_classes<std::shared_ptr<resourceType>>();                                                                                      \
	rttr::type::register_converter_func([](std::shared_ptr<resourceType> ptr, bool& ok) -> std::shared_ptr<Resource> {                                                             \
		ok = true;                                                                                                                                                                 \
		return std::static_pointer_cast<Resource>(ptr);                                                                                                                            \
	});                                                                                                                                                                            \
	rttr::type::register_equal_comparator<ResourceHandle<resourceType>>()


/**
 * Registration of concrete resource handle.
 *
 * @param resourceType
 */
#define REGISTRATION_RESOURCE_HANDLE(resourceType)                                                                                                                                 \
	rttr::registration::class_<ResourceHandle<resourceType>>((resourceType::GetResrourceTypeName() + "Handle").c_str())                                                            \
		.constructor<>()(rttr::policy::ctor::as_object)                                                                                                                            \
		.method("AfterDeserialize", &ResourceHandle<resourceType>::AfterDeserialize)()

// #include <Core/Resources/ResourceManager.h>
#define DECLARE_RESOURCE_TYPE(resourceType)                                                                                                                                        \
	RTTR_REGISTRATION                                                                                                                                                              \
	{                                                                                                                                                                              \
		using namespace GLEngine::Core;                                                                                                                                            \
		REGISTRATION_RESOURCE_HANDLE(resourceType);                                                                                                                                \
                                                                                                                                                                                   \
		if constexpr (requires { requires resourceType::GetResourceDataPath() != ""; })                                                                                            \
		{                                                                                                                                                                          \
			rttr::registration::class_<resourceType>(#resourceType)                                                                                                                \
				.method("GetResourceTypeHash", &resourceType::GetResourceTypeHash)                                                                                                 \
				.method("GetResourceTypeHashStatic", &resourceType::GetResourceTypeHashStatic)                                                                                     \
				.method("GetResourceDataPath", &resourceType::GetResourceDataPath)                                                                                                 \
				.constructor<>()(rttr::policy::ctor::as_std_shared_ptr);                                                                                                           \
		}                                                                                                                                                                          \
		else                                                                                                                                                                       \
		{                                                                                                                                                                          \
			rttr::registration::class_<resourceType>(#resourceType)                                                                                                                \
				.method("GetResourceTypeHash", &resourceType::GetResourceTypeHash)                                                                                                 \
				.method("GetResourceTypeHashStatic", &resourceType::GetResourceTypeHashStatic)                                                                                     \
				.constructor<>()(rttr::policy::ctor::as_std_shared_ptr);                                                                                                           \
		}                                                                                                                                                                          \
                                                                                                                                                                                   \
		DECLARE_RESOURCE_CONVERTOR(resourceType);                                                                                                                                  \
	}

namespace GLEngine::Core {
/**
 * Use inside the resource class body/
 * 
 * @param resourceType 
 */
#define DEFINE_RESOURCE_TYPE(resourceType)                                                                                                                                         \
public:                                                                                                                                                                            \
	inline static constexpr std::string_view GetResourceDataPath()                                                                                                                 \
	{                                                                                                                                                                              \
		return {};                                                                                                                                                                 \
	}                                                                                                                                                                              \
	inline static std::size_t GetResourceTypeHashStatic()                                                                                                                          \
	{                                                                                                                                                                              \
		static std::size_t hash = std::hash<std::string>{}(#resourceType);                                                                                                         \
		return hash;                                                                                                                                                               \
	}                                                                                                                                                                              \
	inline static std::string& GetResrourceTypeName()                                                                                                                              \
	{                                                                                                                                                                              \
		static std::string name(#resourceType);                                                                                                                                    \
		return name;                                                                                                                                                               \
	}                                                                                                                                                                              \
	std::size_t GetResourceTypeHash() const override                                                                                                                               \
	{                                                                                                                                                                              \
		return resourceType::GetResourceTypeHashStatic();                                                                                                                          \
	}                                                                                                                                                                              \
	RTTR_ENABLE(Core::Resource)                                                                                                                                                    \
public:

// For dataPath use variables from @GLEngine::Core::Filesystem
#define DEFINE_RESOURCE_WITH_PATH_TYPE(resourceType, dataPath)                                                                                                                     \
public:                                                                                                                                                                            \
	inline static constexpr const std::string_view GetResourceDataPath()                                                                                                           \
	{                                                                                                                                                                              \
		return dataPath;                                                                                                                                                           \
	}                                                                                                                                                                              \
	inline static std::size_t GetResourceTypeHashStatic()                                                                                                                          \
	{                                                                                                                                                                              \
		static std::size_t hash = std::hash<std::string>{}(#resourceType);                                                                                                         \
		return hash;                                                                                                                                                               \
	}                                                                                                                                                                              \
	inline static std::string& GetResrourceTypeName()                                                                                                                              \
	{                                                                                                                                                                              \
		static std::string name(#resourceType);                                                                                                                                    \
		return name;                                                                                                                                                               \
	}                                                                                                                                                                              \
	std::size_t GetResourceTypeHash() const override                                                                                                                               \
	{                                                                                                                                                                              \
		return resourceType::GetResourceTypeHashStatic();                                                                                                                          \
	}                                                                                                                                                                              \
	RTTR_ENABLE(Core::Resource)                                                                                                                                                    \
public:


enum class ResourceState : std::uint8_t {
	Empty,
	Loading,
	Ready,
	Failed, // cannot load/no loader
};

class Resource;

template <typename T> concept IsBeDerivedResource = requires(T t)
{
	requires std::derived_from<T, Resource>;
	requires std::derived_from<typename T::T_BaseResource, Resource>;
	{
		t.IsDerived()
	} -> std::convertible_to<bool>; // defined as static function
};

template <class T, class TBaseRes> concept IsBuildableResource = requires(T t, TBaseRes b)
{
	{
		t.Build(b)
	} -> std::convertible_to<bool>;
};

template <typename T> concept BuildableResource = requires(T t)
{
	requires IsBeDerivedResource<T>&& IsBuildableResource<T, typename T::T_BaseResource>;
};

class I_ResourceLoader;

// derived resources should take base resource as only constructor argument
class CORE_API_EXPORT Resource : public I_EventReceiver {
public:
	Resource();
	~Resource() override;

	[[nodiscard]] virtual std::unique_ptr<I_ResourceLoader> GetLoader()									= 0;
	[[nodiscard]] virtual bool								Load(const std::filesystem::path& filepath) = 0;
	[[nodiscard]] virtual bool								Reload()									= 0;
	[[nodiscard]] virtual std::size_t						GetResourceTypeHash() const					= 0;

	[[nodiscard]] bool Save() const
	{
		if (!m_Dirty)
			return true;
		if (SupportSaving())
			return SaveInternal();
		return false;
	}

	[[nodiscard]] ResourceState GetState() const;
	[[nodiscard]] bool			IsReady() const;

	virtual bool SupportSaving() const { return false; }
	void		 OnEvent(I_Event& event) override {}

	const std::filesystem::path& GetFilePath() const;

	RTTR_ENABLE()
protected:
	std::filesystem::path m_Filepath;

	virtual bool SaveInternal() const { return false; }

	bool m_Dirty = false;

private:
	ResourceState m_State = ResourceState::Empty;
	friend class C_ResourceManager;
	friend class ResourceHandleFixture;

	RTTR_REGISTRATION_FRIEND
};

template <class ResourceType> concept is_resource = std::is_base_of_v<Resource, ResourceType>;
} // namespace GLEngine::Core