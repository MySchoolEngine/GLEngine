#include <CoreTestStdafx.h>

#include <Core/Resources/ResourceManager.h>

#include <Utils/Serialization/XMLDeserialize.h>

#include <CoreTest/Resources/TestClasses/DelayFileTestResource.h>

DECLARE_RESOURCE_HANDLE_AFTER_DESERIALIZE(DelayFileTestResource)

namespace GLEngine::Core {

std::shared_ptr<Resource> TestResourceLoader::CreateResource() const
{
	return std::make_shared<DelayFileTestResource>();
}

std::vector<std::string> TestResourceLoader::GetSupportedExtensions() const
{
	return {".test", ".test-slow"};
}
} // namespace GLEngine::Core
DECLARE_RESOURCE_TYPE(GLEngine::Core::DelayFileTestResource)