#pragma once

#include <Core/CoreApi.h>

namespace GLEngine::Core {
class Resource;

enum class ResourceState : std::uint8_t
{
	Empty,
	Loading,
	Ready,
	Failed, // cannot load/no loader
};

class CORE_API_EXPORT ResourceHandleBase {
public:
	ResourceHandleBase();
	ResourceHandleBase(std::shared_ptr<Resource> resource);
	virtual ~ResourceHandleBase();
	[[nodiscard]] ResourceState GetState() const;

private:
	std::shared_ptr<Resource> m_Resource; // nullptr only when no loader exists
};

template <class ResourceType, typename = std::enable_if_t<std::is_base_of_v<Resource, ResourceType>>> 
class ResourceHandle : public ResourceHandleBase {
public:
	ResourceHandle() = default;
	ResourceHandle(std::shared_ptr<Resource> resource)
		: ResourceHandleBase(resource)
	{}
};
} // namespace GLEngine::Core
