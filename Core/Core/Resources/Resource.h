#pragma once

#include <Core/CoreApi.h>

namespace GLEngine::Core {

#define DECLARE_RESOURCE_TYPE(resourceType)                                                                                                                                        \
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
	}


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
	[[nodiscard]] virtual bool Reload() = 0;

	[[nodiscard]] ResourceState GetState() const;
	[[nodiscard]] bool			IsReady() const;

private:
	ResourceState m_State = ResourceState::Empty;
	friend class C_ResourceManager;
};

template <class ResourceType> concept is_resource = std::is_base_of_v<Resource, ResourceType>;
} // namespace GLEngine::Core
