#pragma once

#include <Core/Resources/ResourceHandle.h>

namespace GLEngine::Core {
// Resource::m_State is private with `friend class ResourceManagerBaseFixture;` (see
// Core/Resources/Resource.h) so tests can build resource graphs entirely in memory — without
// going through C_ResourceManager or touching disk — by constructing a resource directly and
// marking it Ready here.
class ResourceManagerBaseFixture {
public:
	template <class ResourceType> static ResourceHandle<ResourceType> CreateResourceHandle(std::shared_ptr<ResourceType> resource)
	{
		resource->m_State = ResourceState::Ready;
		return ResourceHandle<ResourceType>(resource);
	}
};
} // namespace GLEngine::Core
