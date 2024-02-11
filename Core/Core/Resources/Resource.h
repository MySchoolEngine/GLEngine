#pragma once

#include <Core/CoreApi.h>

#include <rttr/registration_friend.h>

#define DECLARE_RESOURCE_TYPE(resourceType)                                                                                                                                        \
	RTTR_REGISTRATION                                                                                                                                                              \
	{                                                                                                                                                                              \
		using namespace GLEngine::Core;                                                                                                                                            \
		rttr::registration::class_<ResourceHandle<resourceType>>((resourceType::GetResrourceTypeName() + "Handle").c_str())                                                        \
			.constructor<>()(rttr::policy::ctor::as_object)                                                                                                                        \
			.method("AfterDeserialize", &ResourceHandle<resourceType>::AfterDeserialize)();                                                                                        \
                                                                                                                                                                                   \
		rttr::registration::class_<resourceType>(#resourceType).constructor<>()(rttr::policy::ctor::as_std_shared_ptr);                                                            \
                                                                                                                                                                                   \
		rttr::type::register_wrapper_converter_for_base_classes<std::shared_ptr<resourceType>>();                                                                                  \
		rttr::type::register_converter_func([](std::shared_ptr<resourceType> ptr, bool& ok) -> std::shared_ptr<Resource> {                                                         \
			ok = true;                                                                                                                                                             \
			return std::static_pointer_cast<Resource>(ptr);                                                                                                                        \
		});                                                                                                                                                                        \
	}

namespace GLEngine::Core {

#define DEFINE_RESOURCE_TYPE(resourceType)                                                                                                                                         \
public:                                                                                                                                                                            \
	static std::size_t GetResourceTypeHash()                                                                                                                                       \
	{                                                                                                                                                                              \
		static std::size_t hash = std::hash<std::string>{}(#resourceType);                                                                                                         \
		return hash;                                                                                                                                                               \
	}                                                                                                                                                                              \
	static std::string& GetResrourceTypeName()                                                                                                                                     \
	{                                                                                                                                                                              \
		static std::string name(#resourceType);                                                                                                                                    \
		return name;                                                                                                                                                               \
	}                                                                                                                                                                              \
	RTTR_ENABLE(Core::Resource)                                                                                                                                                    \
public:


enum class ResourceState : std::uint8_t
{
	Empty,
	Loading,
	Ready,
	Failed, // cannot load/no loader
};

class CORE_API_EXPORT Resource {
public:
	Resource();
	virtual ~Resource();

	[[nodiscard]] virtual bool Load(const std::filesystem::path& filepath) = 0;
	[[nodiscard]] virtual bool Reload()									   = 0;

	[[nodiscard]] ResourceState GetState() const;
	[[nodiscard]] bool			IsReady() const;


	const std::filesystem::path& GetFilePath() const;

	RTTR_ENABLE()
protected:
	std::filesystem::path m_Filepath;

private:
	ResourceState m_State = ResourceState::Empty;
	friend class C_ResourceManager;

	RTTR_REGISTRATION_FRIEND
};

template <class ResourceType> concept is_resource = std::is_base_of_v<Resource, ResourceType>;
} // namespace GLEngine::Core
