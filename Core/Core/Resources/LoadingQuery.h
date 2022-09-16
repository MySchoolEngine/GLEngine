#pragma once

#include <Core/CoreApi.h>
#include <Core/Resources/ResourceHandle.h>

namespace GLEngine::Core {
class CORE_API_EXPORT LoadingQuery {
public:
	void AddHandle(ResourceHandleBase handle);
	[[nodiscard]] bool IsDone() const;
	[[nodiscard]] explicit operator bool() const { return IsDone(); }

private:
	std::vector<ResourceHandleBase> m_handles;
};
}
