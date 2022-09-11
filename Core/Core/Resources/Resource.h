#pragma once

#include <Core/CoreApi.h>
#include <Core/Resources/ResourceHandle.h>

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

} // namespace GLEngine::Core
