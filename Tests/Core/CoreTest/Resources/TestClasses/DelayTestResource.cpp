#include <CoreTestStdafx.h>

#include <Core/Resources/ResourceManager.h>

#include <CoreTest/Resources/TestClasses/DelayTestResource.h>

DECLARE_RESOURCE_HANDLE_AFTER_DESERIALIZE(DelayTestResource)

namespace GLEngine::Core {
std::shared_ptr<Resource> DelayTestResourceLoader::CreateResource() const
{
	return std::make_shared<DelayTestResource>();
}

std::vector<std::string> DelayTestResourceLoader::GetSupportedExtensions() const
{
	return {".test", ".test-slow"};
}
} // namespace GLEngine::Core
DECLARE_RESOURCE_TYPE(GLEngine::Core::DelayTestResource)